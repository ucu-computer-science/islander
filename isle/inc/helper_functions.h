#ifndef NAMESPACES_HELPER_FUNCTIONS_H
#define NAMESPACES_HELPER_FUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <memory.h>

#include "./base_header.h"
#include "./manage_data/manage_mount.h"
#include "./manage_data/manage_volumes.h"
#include "./manage_data/manage_tmpfs.h"
#include "./manage_data/manage_remote_volumes.h"

#define PIPE_FD_NUM 2
#define PIPE_OK_MSG "OK"
#define PIPE_MSG_SIZE 2
#define COMMAND_SIZE MAX_PATH_LENGTH

static void kill_process(const char *msg, ...) {
    va_list params;

    va_start(params, msg);
    vfprintf(stderr, msg, params);
    va_end(params);
    exit(1);
}

void parse_args(int argc, char** argv, struct process_params *params, resource_limits *res_limits);

void set_up_default_params(struct process_params* params);

void enable_features(int isle_pid, struct process_params *params, const char *exec_file_path);

void release_resources(int isle_pid, struct process_params *params);

char* substr(const char *src, int m, int n);

void remove_file(char *file_path);

void write_file(char path[100], char line[100]);

char* get_username();

void get_islander_home(char *islander_home_path, const char *exec_file_path);

void get_aws_secrets_path(char *aws_secrets_path, const char *exec_file_path);

void get_az_secrets_path(char *az_secrets_path, const char *exec_file_path);

void get_gcp_secrets_path(char *gcp_secrets_path, const char *exec_file_path);

void exec_bash_cmd(char *cmd);

void await_setup(int pipe);

void get_cgroup_name(char *dest_str, int pid);

void str_array_concat(char *dest_str, char *strings[], int strings_size);

void create_dir(char* subsystem_path);

void create_islenode(char* isle_name, int isle_pid, char *exec_file_path);

#endif //NAMESPACES_HELPER_FUNCTIONS_H

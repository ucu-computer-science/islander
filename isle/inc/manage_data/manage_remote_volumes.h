#ifndef ISLANDER_ENGINE_MANAGE_REMOTE_VOLUMES_H
#define ISLANDER_ENGINE_MANAGE_REMOTE_VOLUMES_H

#include "../base_header.h"
#include "../helper_functions.h"

void mount_s3_bucket(int isle_pid, char* src_bucket_name, char* dest_bucket_path, const char *exec_file_path);

void exec_s3_cmd(char *operation, char *bucket_name);

#endif //ISLANDER_ENGINE_MANAGE_REMOTE_VOLUMES_H

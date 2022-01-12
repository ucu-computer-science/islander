#ifndef ISLANDER_ENGINE_MANAGE_REMOTE_VOLUMES_H
#define ISLANDER_ENGINE_MANAGE_REMOTE_VOLUMES_H

#include "../base_header.h"
#include "../helper_functions.h"
#include "../manage_data/manage_mount.h"

void mount_s3_bucket(int isle_pid, char* src_bucket_name, char* dest_bucket_path, const char *exec_file_path);

void exec_s3_cmd(char *operation, char *bucket_name);

void umount_cloud_dir(int isle_pid, char* dest_dir_path);

#endif //ISLANDER_ENGINE_MANAGE_REMOTE_VOLUMES_H

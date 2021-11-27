#ifndef MANAGE_VOLUMES_H
#define MANAGE_VOLUMES_H


void volume_feature(int isle_pid, struct process_params *params);

void unmount_volumes(int isle_pid, struct process_params *params);

void mount_ns_volume(int isle_pid, char* src_dir_path, char* dest_dir_path);

void unmount_ns_volume(int isle_pid, char* dest_dir_path);

#endif //MANAGE_VOLUMES_H

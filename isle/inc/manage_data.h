#ifndef NAMESPACES_MANAGE_DATA_H
#define NAMESPACES_MANAGE_DATA_H


void mount_feature(int isle_pid, struct process_params *params);

void unmount_dirs(int isle_pid, struct process_params *params);

void mount_ns_dir(int isle_pid, char* src_dir_path, char* dest_dir_path);

void unmount_ns_dir(int isle_pid, char* dest_dir_path);

#endif //NAMESPACES_MANAGE_DATA_H

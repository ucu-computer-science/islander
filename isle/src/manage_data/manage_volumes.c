#include "../../inc/base_header.h"
#include "../../inc/helper_functions.h"
#include "../../inc/manage_data/manage_volumes.h"
#include "../../inc/manage_data/manage_mount.h"


void volume_feature(int isle_pid, struct process_params *params) {
//    char* src_dir_path = "/dev/";
//    char* dest_dir_path = "../ubuntu-rootfs/host_dev/";
    for (int i = 0; i < params->vlm_num; i++) {
        printf("volume_feature: src -- %s, dest -- %s\n", params->vlm_src[i],  params->vlm_dst[i]);
        mount_ns_volume(isle_pid, params->vlm_src[i], params->vlm_dst[i]);
    }
}


void unmount_volumes(int isle_pid, struct process_params *params) {
//    char* dest_dir_path = "../ubuntu-rootfs/host_dev/";
    for (int i = 0; i < params->vlm_num; i++) {
        printf("unvolume_dirs: dest -- %s\n", params->vlm_dst[i]);
        unmount_ns_dir(isle_pid, params->vlm_dst[i]);
    }
}


void mount_ns_volume(int isle_pid, char* src_vlm_name, char* dest_vlm_path) {
    char *str_arr[] = {SRC_VOLUMES_PATH, src_vlm_name};
    char abs_vlm_path[256];
    abs_vlm_path[0] = '\0';
    str_array_concat(abs_vlm_path, str_arr, 2);
    printf("abs_vlm_path -- %s\n", abs_vlm_path);

    char *str_arr2[] = {"btrfs subvolume create ", abs_vlm_path};
    char btrfs_cmd[256];
    btrfs_cmd[0] = '\0';
    str_array_concat(btrfs_cmd, str_arr2, 2);
    printf("btrfs_cmd -- %s\n", btrfs_cmd);

    FILE *cmd = popen(btrfs_cmd, "r");

    char result[128];

    while (fgets(result, sizeof(result), cmd) != NULL)
           printf("%s", result);
    pclose(cmd);

    mount_ns_dir(isle_pid, abs_vlm_path, dest_vlm_path);
}

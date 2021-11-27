#include "../inc/base_header.h"
#include "../inc/manage_volumes.h"
#include "../inc/define_vars.h"


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
        unmount_ns_volume(isle_pid, params->vlm_dst[i]);
    }
}


void mount_ns_volume(int isle_pid, char* src_dir_path, char* dest_dir_path) {
    char *str_arr[] = {SRC_VOLUMES_PATH, res_limits->device_read_bps};
    char read_bps_device_value[256];
    read_bps_device_value[0] = '\0';
    str_array_concat(read_bps_device_value, str_arr2, 2);

    FILE *cmd = popen("btrfs subvolume create /var/lib/islander/volumes/test_volume3", "r");

    char result[128];

    while (fgets(result, sizeof(result), cmd) != NULL)
           printf("%s", result);
    pclose(cmd);
}


void unmount_ns_volume(int isle_pid, char* dest_dir_path) {}

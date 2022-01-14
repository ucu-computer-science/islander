#include "../inc/base_header.h"
#include "../inc/s3_interaction.h"
#include "../inc/az_interaction.h"


int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Incorrect number of args. It should be: ./remote-vlm-manager <OPERATION> <CLOUD> <BUCKET_NAME>");
        exit(EXIT_FAILURE);
    }

    // get bucket name and operation to perform
    std::string operation = argv[1];
    std::string cloud_provider = argv[2];
    std::string bucket_name = argv[3];

    // set exec_path to current working dir to use exec_path for getting substring with user host path,
    // in case we run islander_engine binary with relative path to it
    std::string cwd = std::filesystem::current_path();

    // get substring with user host path
    uint count = 0;
    uint substr_len = 0;
    for (uint i = 0; i < cwd.size(); i++) {
        if (cwd[i] == '/') {
            if (++count == 3) {
                substr_len = i + 1;
                break;
            }
        }
    }
    std::string user_home_path = cwd.substr(0, substr_len);

    // execute terraform script
    if (operation == CREATE_VLM) {
        if (cloud_provider == AWS_CLOUD) create_s3_bucket(user_home_path, bucket_name);
        else if (cloud_provider == AZ_CLOUD) create_az_storage_container(user_home_path, bucket_name);
    }
    else if (operation == DELETE_VLM) {
        if (cloud_provider == AWS_CLOUD) delete_s3_bucket(user_home_path, bucket_name);
        else if (cloud_provider == AZ_CLOUD) delete_az_storage_container(user_home_path, bucket_name);
    }
    else {
        fprintf(stderr, "You input incorrect operation or cloud provider\n");
    }

    return 0;
}

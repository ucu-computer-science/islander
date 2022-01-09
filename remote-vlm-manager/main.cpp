#include <unistd.h>
#include <iostream>
#include <filesystem>

// cloud providers
#define AWS_CLOUD "aws"

// operations
#define CREATE_VLM "create"
#define DELETE_VLM "delete"

#define S3_TERRAFORM_PATH "islander/remote-volumes/s3_terraform/"


std::string exec_bash_command(const std::string &cmd, int& out_exitStatus) {
    out_exitStatus = 0;
    auto pPipe = ::popen(cmd.c_str(), "r");
    if (pPipe == nullptr) {
        throw std::runtime_error("Cannot open pipe");
    }

    std::array<char, 256> buffer{};
    std::string result;

    while (not std::feof(pPipe))
    {
        auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
        result.append(buffer.data(), bytes);
    }

    auto rc = ::pclose(pPipe);

    if (WIFEXITED(rc)) {
        out_exitStatus = WEXITSTATUS(rc);
    }

    return result;
}


void create_s3_bucket(std::string &user_home_path, std::string &bucket_name) {
    std::string s3_trf_path = S3_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + s3_trf_path;
    std::cout << trf_scripts_path << std::endl;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Creating s3 bucket..." << std::endl;
    std::string terraform_cmd = "terraform init > /dev/null";
//        std::string terraform_cmd = "terraform init";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);
//        std::cout << "Result: " << result << std::endl;
//        FILE *cmd = popen(terraform_cmd.c_str(), "r");
//        pclose(cmd);

//        terraform_cmd = "terraform apply -var bucket_name=" + bucket_name + " > /dev/null 2>&1";
    terraform_cmd = "terraform plan -out s3.plan -var bucket_name=\"" + bucket_name + "\"";
    std::cout << "trf apply -- " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);
//    std::cout << "Result: " << result << std::endl;

    terraform_cmd = "terraform apply s3.plan";
    std::cout << "trf apply -- " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
//        FILE *cmd2 = popen(terraform_cmd.c_str(), "r");
//        pclose(cmd2);
}


void delete_s3_bucket(std::string &user_home_path, std::string &bucket_name) {
    std::string s3_trf_path = S3_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + s3_trf_path;
    std::cout << trf_scripts_path << std::endl;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Deleting s3 bucket..." << std::endl;
    std::string terraform_cmd = "terraform destroy -var bucket_name=\"" + bucket_name + "\" -auto-approve";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Incorrect number of args. It should be: ./remote-vlm-manager <OPERATION> <CLOUD> <BUCKET_NAME>");
        exit(EXIT_FAILURE);
    }

    // save credentials in file

    // get bucket name and operation to perform
    std::string operation = argv[1];
    std::string cloud_provider = argv[2];
    std::string bucket_name = argv[3];

    // set exec_path to current working dir to use exec_path for getting substring with user host path,
    // in case we run islander_engine binary with relative path to it
    std::string cwd = std::filesystem::current_path();
    std::cout << "cwd -- " << cwd << std::endl;

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
    }
    else if (operation == DELETE_VLM) {
        if (cloud_provider == AWS_CLOUD) delete_s3_bucket(user_home_path, bucket_name);
    }
    else {
        fprintf(stderr, "You input incorrect operation or cloud provider");
    }

    // delete bucket with terraform

    return 0;
}

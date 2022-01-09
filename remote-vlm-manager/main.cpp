#include <unistd.h>
#include <iostream>
#include <filesystem>

#define CREATE_VLM "create"

std::string exec_command(const std::string &cmd, int& out_exitStatus) {
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


int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Incorrect number of args. It should be: ./remote-vlm-manager <OPERATION> <CLOUD> <BUCKET_NAME>");
        exit(EXIT_FAILURE);
    }

    // save credentials in file

    // get bucket name and operation to perform
    std::string operation = argv[1];
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
        std::string trf_scripts_path = user_home_path + "islander/remote-volumes/s3_terraform/";
        std::cout << trf_scripts_path << std::endl;
        std::filesystem::current_path(trf_scripts_path);

//        std::string terraform_cmd = "terraform init > /dev/null 2>&1";
        std::string terraform_cmd = "terraform init";
        int exit_status = 0;
        auto result = exec_command(terraform_cmd, exit_status);
//        std::cout << "Result: " << result << std::endl;
//        FILE *cmd = popen(terraform_cmd.c_str(), "r");
//        pclose(cmd);

//        terraform_cmd = "terraform apply -var bucket_name=" + bucket_name + " > /dev/null 2>&1";
        terraform_cmd = "terraform apply -var bucket_name=\"" + bucket_name + "\" -auto-approve";
        std::cout << "trf apply -- " << terraform_cmd << std::endl;
        result = exec_command(terraform_cmd, exit_status);
        std::cout << "Result: " << result << std::endl;
//        FILE *cmd2 = popen(terraform_cmd.c_str(), "r");
//        pclose(cmd2);
    }

    // delete bucket with terraform

    return 0;
}

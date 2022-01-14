#include "../inc/az_interaction.h"
#include "../inc/utils.h"
#include "../inc/s3_interaction.h"

/** Use terraform script to create Azure storage container */
void create_az_storage_container(std::string &user_home_path, std::string &container_name) {
    std::string s3_trf_path = AZ_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + s3_trf_path;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Creating Azure storage container..." << std::endl;
    std::string terraform_cmd = "terraform init > /dev/null";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);

    terraform_cmd = "terraform plan -out az.plan -var container_name=\"" + container_name + "\"";
    std::cout << "Execute command: " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);

    terraform_cmd = "terraform apply az.plan";
    std::cout << "Execute command: " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}


/** Use terraform script to delete Azure storage container */
void delete_az_storage_container(std::string &user_home_path, std::string &bucket_name) {
    std::string s3_trf_path = AZ_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + s3_trf_path;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Deleting Azure storage container..." << std::endl;
    std::string terraform_cmd = "terraform destroy -var container_name=\"" + bucket_name + "\" -auto-approve";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}

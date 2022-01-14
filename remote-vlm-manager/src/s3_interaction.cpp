#include "../inc/utils.h"
#include "../inc/s3_interaction.h"


/** Use terraform script to create S3 bucket */
void create_s3_bucket(std::string &user_home_path, std::string &bucket_name) {
    std::string s3_trf_path = S3_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + s3_trf_path;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Creating s3 bucket..." << std::endl;
    std::string terraform_cmd = "terraform init > /dev/null";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);

    terraform_cmd = "terraform plan -out s3.plan -var bucket_name=\"" + bucket_name + "\"";
    std::cout << "Execute command: " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);

    terraform_cmd = "terraform apply s3.plan";
    std::cout << "Execute command: " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}


/** Use terraform script to delete S3 bucket */
void delete_s3_bucket(std::string &user_home_path, std::string &bucket_name) {
    std::string s3_trf_path = S3_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + s3_trf_path;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Deleting s3 bucket..." << std::endl;
    std::string terraform_cmd = "terraform destroy -var bucket_name=\"" + bucket_name + "\" -auto-approve";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}

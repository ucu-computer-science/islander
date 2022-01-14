#include "../inc/gcp_interaction.h"

/** Use terraform script to create GCP bucket */
void create_gcp_bucket(std::string &user_home_path, std::string &bucket_name) {
    // Set GOOGLE_APPLICATION_CREDENTIALS to authenticate to GCP account
    std::string full_gcp_secrets_path = user_home_path + SECRETS_PREFIX + GCP_SECRETS_NAME;
    setenv("GOOGLE_APPLICATION_CREDENTIALS", full_gcp_secrets_path.c_str(), 1);

    std::string gcp_trf_path = GCP_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + gcp_trf_path;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Creating GCP bucket..." << std::endl;
    std::string terraform_cmd = "terraform init > /dev/null";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);

    terraform_cmd = "terraform plan -out gcp.plan -var bucket_name=\"" + bucket_name + "\"";
    std::cout << "Execute command: " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);

    terraform_cmd = "terraform apply gcp.plan";
    std::cout << "Execute command: " << terraform_cmd << std::endl;
    result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}


/** Use terraform script to delete GCP bucket */
void delete_gcp_bucket(std::string &user_home_path, std::string &bucket_name) {
    std::string gcp_trf_path = GCP_TERRAFORM_PATH;
    std::string trf_scripts_path = user_home_path + gcp_trf_path;
    std::filesystem::current_path(trf_scripts_path);

    std::cout << "Deleting GCP bucket..." << std::endl;
    std::string terraform_cmd = "terraform destroy -var bucket_name=\"" + bucket_name + "\" -auto-approve";
    int exit_status = 0;
    auto result = exec_bash_command(terraform_cmd, exit_status);
    std::cout << "Output:\n " << result << std::endl;
}

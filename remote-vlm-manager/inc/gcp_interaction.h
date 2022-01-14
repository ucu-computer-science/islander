#ifndef REMOTE_VLM_MANAGER_GCP_INTERACTION_H
#define REMOTE_VLM_MANAGER_GCP_INTERACTION_H

#include "../inc/utils.h"

void create_gcp_bucket(std::string &user_home_path, std::string &bucket_name);

void delete_gcp_bucket(std::string &user_home_path, std::string &bucket_name);

#endif //REMOTE_VLM_MANAGER_GCP_INTERACTION_H

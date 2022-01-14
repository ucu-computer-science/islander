#ifndef REMOTE_VLM_MANAGER_AZ_INTERACTION_H
#define REMOTE_VLM_MANAGER_AZ_INTERACTION_H

#include "./base_header.h"

void create_az_storage_container(std::string &user_home_path, std::string &bucket_name);

void delete_az_storage_container(std::string &user_home_path, std::string &bucket_name);

#endif //REMOTE_VLM_MANAGER_AZ_INTERACTION_H

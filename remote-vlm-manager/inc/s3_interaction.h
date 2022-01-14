#ifndef REMOTE_VLM_MANAGER_S3_INTERACTION_H
#define REMOTE_VLM_MANAGER_S3_INTERACTION_H

#include "./base_header.h"

void create_s3_bucket(std::string &user_home_path, std::string &bucket_name);

void delete_s3_bucket(std::string &user_home_path, std::string &bucket_name);

#endif //REMOTE_VLM_MANAGER_S3_INTERACTION_H

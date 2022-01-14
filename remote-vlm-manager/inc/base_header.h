#ifndef REMOTE_VLM_MANAGER_BASE_HEADER_H
#define REMOTE_VLM_MANAGER_BASE_HEADER_H

#include <unistd.h>
#include <iostream>
#include <filesystem>

// cloud providers
#define AWS_CLOUD "aws"
#define AZ_CLOUD "az"

// operations
#define CREATE_VLM "create"
#define DELETE_VLM "delete"

#define S3_TERRAFORM_PATH "islander/remote-volumes/s3_terraform/"
#define AZ_TERRAFORM_PATH "islander/remote-volumes/az_terraform/"

#endif //REMOTE_VLM_MANAGER_BASE_HEADER_H

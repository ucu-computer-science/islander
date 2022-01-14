#ifndef PS_UTILS_H
#define PS_UTILS_H

#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>              /* Definition of O_* constants */
#include <string.h>

#define MAX_FD_NUM 20
#define MAX_PATH_LENGTH 256

#define ISLANDER_HOME_PREFIX "islander/"
#define ISLENODE_DIR_PATH "islenodes/"

void get_islander_home(std::string &islander_home_path);

void get_full_islenodes_path(std::string &full_islenodes_path);

#endif //PS_UTILS_H

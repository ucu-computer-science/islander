#ifndef PS_UTILS_H
#define PS_UTILS_H


#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>              /* Definition of O_* constants */
#include <string.h>

#define MAX_FD_NUM 20
#define MAX_PATH_LENGTH 256

#define ISLANDER_HOME_PREFIX "islander/"
#define LOGGER_DIR_PATH "logger/"
#define LOG_FDS_DIR_PATH "log_fds/"

void get_logger_path(char* path);

#endif //PS_UTILS_H

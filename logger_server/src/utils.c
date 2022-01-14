#include "../inc/utils.h"


void get_logger_path(char* path) {
    const char *exec_path;
    char user_home_path[MAX_PATH_LENGTH];

    // Set exec_path to current working dir to use exec_path for getting substring with user host path,
    // in case we run islander_engine binary with relative path to it
    char cwd[MAX_PATH_LENGTH];
    getcwd(cwd, MAX_PATH_LENGTH);
    exec_path = cwd;

    // get substring with user host path
    int count = 0;
    int substr_len = 0;
    for (int i = 0; i < strlen(exec_path); i++) {
        if (exec_path[i] == '/') {
            if (++count == 3) {
                substr_len = i + 1;
                break;
            }
        }
    }
    strncpy(user_home_path, exec_path, substr_len);
    user_home_path[substr_len] = '\0';

    strcat(user_home_path, ISLANDER_HOME_PREFIX);
    strcat(user_home_path, LOGGER_DIR_PATH);
    strcpy(path, user_home_path);
}
#include "../inc/utils.h"


void get_islander_home(std::string &islander_home_path) {
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

    std::string islander_home = ISLANDER_HOME_PREFIX;
    islander_home_path = user_home_path + islander_home;
}


void get_full_islenodes_path(std::string &full_islenodes_path) {
    std::string islander_home_path;
    get_islander_home(islander_home_path);
    full_islenodes_path = islander_home_path + ISLENODE_DIR_PATH;
}


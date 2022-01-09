#ifndef MYSHELL_SERVER_H
#define MYSHELL_SERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
#include <ctime>
#include <signal.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <boost/algorithm/string.hpp>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include "options_parser.hpp"
#include "utils.hpp"
#include "defined_vars.hpp"
#include "./wrappers.h"

void run_server(int port);

std::string get_working_path();

std::string run_my_shell(std::string& working_dir_path, std::string& command);

#endif //MYSHELL_SERVER_H

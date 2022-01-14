#ifndef MYSHELL_UTILS_HPP
#define MYSHELL_UTILS_HPP

#include <boost/algorithm/string.hpp>
#include <options_parser.hpp>
#include <cstring>
#include <vector>
#include <sstream>
#include <deque>

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

#include "base_header.hpp"

bool is_number(const std::string& str);

void log_action(sockaddr_in* client_info, const std::string& message);

void exit_gracefully(const std::vector<pid_t>& children_pids, int socket_desc, int exit_code);

#endif //MYSHELL_UTILS_HPP

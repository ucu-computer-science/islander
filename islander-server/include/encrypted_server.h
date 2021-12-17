#ifndef MYSHELL_OPENSSL_SERVER_H
#define MYSHELL_OPENSSL_SERVER_H

#include <malloc.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <fcntl.h>
#include <wait.h>

#include <boost/algorithm/string.hpp>
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "./base_header.hpp"
#include "./defined_vars.hpp"
#include "./utils.hpp"


void run_encrypted_server(int port);

[[noreturn]] void process_new_client(SSL* ssl, sockaddr_in* client_info);

void encrypted_process_enclosed_cmd(std::string &cur_cmd, std::string &working_dir_path);

int OpenListener(int port);

int isRoot();

SSL_CTX* InitServerCTX(void);

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);

void ShowCerts(SSL* ssl);

void close_ssl_connection();

#endif //MYSHELL_OPENSSL_SERVER_H

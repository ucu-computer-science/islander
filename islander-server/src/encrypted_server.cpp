// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../include/encrypted_server.h"
#include "../include/execution_functs.hpp"

/* here we use global variable, since ssl var used as descriptor for all read/write operations for encrypted server */
SSL *ssl;

void run_encrypted_server(int port) {
    SSL_CTX *ctx;
    int socket_desc;

    // Only root user have the permission to run the server
    if(!isRoot()) {
        cout << "This program must be run as root/sudo user!!" << endl;
        exit(0);
    }

    /* Signal handling variables */
    sigset_t mask;  // mask contains all of the signals that we need to catch
    int signal_fd;  // specified signals will be read from this fd
    struct signalfd_siginfo fd_signal_info{};  // this struct will contain more info about each read signal
    ssize_t read_signal;

    sigemptyset(&mask);  // Clear mask
    sigaddset(&mask, SIGINT);  // Catch SIGINT signal
    sigaddset(&mask, SIGQUIT);  // Catch SIGQUIT signal

    /* Block signals, so that they aren't handled
       according to their default dispositions. */
    if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1)
        handle_error("sigprocmask");

    signal_fd = signalfd(-1, &mask, SFD_NONBLOCK);
    if (signal_fd == -1)
        handle_error("signalfd");

    // Initialize the SSL library
    SSL_library_init();
    ctx = InitServerCTX();        /* initialize SSL */
    char *certificate_location = "./mycert.pem";
    LoadCertificates(ctx, certificate_location, certificate_location); /* load certs */
    socket_desc = OpenListener(port);    /* create server socket */

    /* make encrypted server as non-blocking */
    if (fcntl(socket_desc, F_SETFL, SOCK_NONBLOCK) == -1) {
        printf("Could not switch to non-blocking.\n");
        exit(FAIL);
    }

    if (socket_desc == -1) {
        encrypted_cerr_if_needed("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    std::vector<pid_t> children_pids;

    /* Server will keep running forever until explicitly interrupted */
    for (;;) {
        // Check for Quit signal
        read_signal = read(signal_fd, &fd_signal_info, sizeof(fd_signal_info));

        // Most of the time read will fail with EAGAIN because read is non-blocking
        // The IF below will only be executed if we received a new signal
        if (read_signal == sizeof(fd_signal_info)) {
            if (fd_signal_info.ssi_signo == SIGINT || fd_signal_info.ssi_signo == SIGQUIT) {
                cout << "\nAbort signal received." << endl;
                close(socket_desc);          /* close server socket */
                SSL_CTX_free(ctx);         /* release context */

                exit_gracefully(children_pids, socket_desc, EXIT_SUCCESS);
            }
        }

        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client_socket_desc = accept4(socket_desc, (struct sockaddr*)&addr, &len, SOCK_NONBLOCK);  /* accept connection as usual */

        // If accept fails, then continue the busy loop
        if (client_socket_desc == -1) {
            if (errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
                encrypted_cerr_if_needed("accept");
            }
            continue;
        }

        // If we are here, a new connection has been successfully created
        log_action(&addr, "New connection");

        std::string accumulated_value;

        // Create a separate process to handle the new client
        pid = fork();

        if (pid == -1) {
            encrypted_cerr_if_needed("fork");
            close(socket_desc);        /* close server socket */
            SSL_CTX_free(ctx);         /* release context */
            exit_gracefully(children_pids, socket_desc, EXIT_FAILURE);
        }
        else if (pid == 0) {
            // The child process
            ssl = SSL_new(ctx);              /* get new SSL state with context */
            SSL_set_fd(ssl, client_socket_desc);      /* set connection socket to SSL state */
            process_new_client(ssl, &addr);         /* service connection */
        }
        children_pids.emplace_back(pid);
    }
}


[[noreturn]] void process_new_client(SSL* ssl, sockaddr_in* client_info) {
    char buf[SOCKET_BUF_SIZE] = {0};
    std::string ServerResponse = "Encryption successful.\n\n";
    ServerResponse += COMMUNICATION_DELIMiTER;
    const char *cpValidMessage = "UserName: test_user, Password: 123\n";

    /* here we need a loop to enable accept with clients,
     * as our server non-blocking and accept means several communication actions*/
    while (true) {
        int err = SSL_accept(ssl);
        if (err == 1) {
            break;
        }
    }

    // printf("SSL connection using %s\n", SSL_get_cipher (ssl));

    std::string accumulated_value;
    bool is_authentication = true;
    ssize_t cc;

    int write_pfd[2];
    int read_pfd[2];

    bool script_launched = false;
    for (;;) {
        // Read from socket until new line character appears
        for (;;) {
            cc = SSL_read(ssl, buf, SOCKET_BUF_SIZE); /* get request */

            if (cc == 0) {
                // Executes if connection is closed gracefully. If closed unexpectedly, it won't be noticed.
                log_action(client_info, "Close connection");
                close_ssl_connection();
                continue;
            } else if (cc == -1) {
                if (errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
                    log_action(client_info, "Error happened. Description below");
                    encrypted_cerr_if_needed("recv");
                }
                continue;
            }
            buf[cc] = '\0';
            accumulated_value.append(buf);

            // Wait until \n appears (it might also appear many times)
            if (accumulated_value.find('\n') != std::string::npos) {
                break;
            }
        }

        // Handle the case if several new line characters are read. We need to execute every command.
        std::vector<std::string> cmd_list;
        boost::split(cmd_list, accumulated_value, boost::is_any_of("\n"));

        accumulated_value = cmd_list.back();
        cmd_list.pop_back();  // Pop all that is left after the last \n

//        std::cout << "here!" << std::endl;

        ssize_t nbytes;
        char inbuf[SOCKET_BUF_SIZE];
        for (auto &cur_cmd: cmd_list) {
            if (!is_authentication) {
                log_action(client_info, cur_cmd);

                if (!script_launched) {
                    script_launched = true;

                    /* create a pipe to encrypt output of external, piped, enclosed commands */
                    pipe(read_pfd);
                    pipe(write_pfd);

                    run_external_command(read_pfd, write_pfd, cur_cmd);
//                    usleep(3'000'000);
//                    std::cout << "wait over" << std::endl;
//                    write(write_pfd[ENCRYPTED_PIPE_WRITE], "ls", 2);

                    std::string to_write;

                    /* read from read end of pipe until EOF (return value 0) */
                    while ((nbytes = read(read_pfd[ENCRYPTED_PIPE_READ], inbuf, SOCKET_BUF_SIZE)) > 0) {
                        inbuf[nbytes] = '\0';
                        to_write += inbuf;
                        to_write += COMMUNICATION_DELIMiTER;
                        SSL_write(ssl, to_write.c_str(), to_write.size());
                    }
                }

                // Write to pipe
                // write(write_pfd[ENCRYPTED_PIPE_WRITE], "ls", 2);
            } else { // authentication block, first action with the client
                is_authentication = false;
                if (strcmp(cpValidMessage, buf) == 0) { // if credentials are correct
                    std::cout << "correct" << std::endl;
                    SSL_write(ssl, ServerResponse.c_str(), ServerResponse.size()); /* send reply */
                } else { // if credentials are incorrect
                    std::cout << "incorrect" << cpValidMessage << buf << std::endl;
                    SSL_write(ssl, INVALID_CREDENTIALS, strlen(INVALID_CREDENTIALS));
                    encrypted_cout_if_needed(COMMUNICATION_DELIMiTER);
                    close_ssl_connection();
                }
            }
        }
    }
}

// Create the SSL socket and initialize the socket address structure
int OpenListener(int port) {
    int sd;
    struct sockaddr_in addr;
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Can't configure listening port");
        abort();
    }
    return sd;
}


int isRoot() {
    if (getuid() != 0) return 0;
    else return 1;
}


SSL_CTX* InitServerCTX(void) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == nullptr )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}


void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}


void ShowCerts(SSL* ssl) {
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != nullptr ) {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), nullptr, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), nullptr, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}


void close_ssl_connection() {
    int sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
    exit(EXIT_SUCCESS);
}

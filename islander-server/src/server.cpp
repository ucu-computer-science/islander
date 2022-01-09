// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../include/server.h"


void run_server(int port) {
    struct sockaddr_in server {};

    /* Create server socket */
    int socket_desc = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(socket_desc == -1) {
        encrypted_cerr_if_needed("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    /* Signal handling variables */
    sigset_t mask;  // mask contains all of the signals that we need to catch
    int signal_fd;  // specified signals will be read from this fd
    struct signalfd_siginfo fd_signal_info {};  // this struct will contain more info about each read signal
    ssize_t read_signal;

    sigemptyset(&mask);  // Clear mask
    sigaddset(&mask, SIGINT);  // Catch SIGINT signal
    sigaddset(&mask, SIGQUIT);  // Catch SIGQUIT signal

    /* Block signals so that they aren't handled
       according to their default dispositions. */
    if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1)
        handle_error("sigprocmask");

    signal_fd = signalfd(-1, &mask, SFD_NONBLOCK);
    if (signal_fd == -1)
        handle_error("signalfd");

    /* Zeroing server_addr struct */
    memset(&server, 0, sizeof(server));

    /* Set appropriate connection values */
    server.sin_family = AF_INET;  // AF_INET instructs to use IPv6 connection
    inet_pton(AF_INET, SERVER_ADDRESS, &(server.sin_addr));
    server.sin_port = htons(port);

    /* Bind to the address & port */
    int res = bind(socket_desc, (struct sockaddr *) &server, sizeof(server));
    if (res == -1) {
        encrypted_cerr_if_needed("Unable to bind to address");
        exit(EXIT_FAILURE);
    }

    // Start listening at socket_desc, 10 is a backlog parameter
    listen(socket_desc, 10);

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
                exit_gracefully(children_pids, socket_desc, EXIT_SUCCESS);
            }
        }

        // Define client connection struct
        struct sockaddr_in client_info = {};
        socklen_t size = sizeof(client_info);

        // Make accept non-blocking. IF we accept a connection, a new client socket is created
        int client_socket_desc = accept4(socket_desc, (sockaddr*)& client_info, &size, SOCK_NONBLOCK);

        // If accept fails, then continue the busy loop
        if (client_socket_desc == -1) {
            if (errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
                encrypted_cerr_if_needed("accept4");
            }
            continue;
        }

        // If we are here, a new connection has been successfully created
        log_action(&client_info, "New connection");

        char buf [SOCKET_BUF_SIZE];
        std::string accumulated_value;

        // Create a separate process to handle the new client
        pid = fork();

        if (pid == -1) {
            encrypted_cerr_if_needed("fork");
            exit_gracefully(children_pids, socket_desc, EXIT_FAILURE);
        }
        else if (pid == 0) {
            // The child process

            // Redirect stdout & stderr to socket fd
            if (dup2(client_socket_desc, STDOUT_FILENO) == -1) {
                encrypted_cerr_if_needed("dup2: failed to duplicate stdout on socket fd");
                exit(EXIT_FAILURE);
            }
            if (dup2(client_socket_desc, STDERR_FILENO) == -1) {
                encrypted_cerr_if_needed("dup2: failed to duplicate stderr on socket fd");
                exit(EXIT_FAILURE);
            }

            auto working_dir_path = get_working_path();
            ssize_t cc;
            for (;;) {
                // Read from socket until new line character appears
                for (;;) {
                    cc = recv(client_socket_desc, buf, sizeof(buf), 0);

                    if (cc == 0) {
                        // Executes if connection is closed gracefully. If closed unexpectedly, it won't be noticed.
                        log_action(&client_info, "Close connection");
                        close(client_socket_desc);
                        exit(EXIT_SUCCESS);
                    } else if (cc == -1) {
                        if (errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
                            log_action(&client_info, "Error happened. Description below");
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
                for (auto& cur_cmd : cmd_list) {
#ifdef DEBUG_MODE
                    log_action(&client_info, cur_cmd);
#endif
//                    working_dir_path = run_my_shell(working_dir_path, cur_cmd);
                    encrypted_cout_if_needed(COMMUNICATION_DELIMiTER);
                }
            }
        }

        children_pids.emplace_back(pid);
    }
}


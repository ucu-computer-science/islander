#include "../inc/server.h"
#include "../inc/utils.h"
#include <fcntl.h>

#define LOGGER_FORMAT ".txt"
#define BUFFER_SIZE 1024


/** Run multi-client server, which logs container output */
int run_server(int argc, char *argv[]) {
    // Get path to where logs reside.
    char home_path[BUFFER_SIZE];
    get_logger_path(home_path);
    home_path[strlen(home_path)] = '\0';

    // Get path to the logger files.
    char loggs_path[BUFFER_SIZE];
    strcpy(loggs_path, home_path);
    strcat(loggs_path, LOGGER_DIR_PATH);
    loggs_path[strlen(loggs_path)] = '\0';

    // Get path to the logger files file descriptors.
    char log_fds_path[BUFFER_SIZE];
    strcpy(log_fds_path, home_path);
    strcat(log_fds_path, LOG_FDS_DIR_PATH);
    log_fds_path[strlen(log_fds_path)] = '\0';

    struct sockaddr_un addr;

    // Create a new server socket with domain: AF_UNIX, type: SOCK_STREAM, protocol: 0
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("Server socket fd = %d\n", sfd);

    // Make sure socket's file descriptor is legit.
    if (sfd == -1) {
        errExit("socket");
    }

    // Make sure the address we're planning to use isn't too long.
    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {
        fatal("Server socket path too long: %s", SV_SOCK_PATH);
    }

    // Delete any file that already exists at the address. Make sure the deletion
    // succeeds. If the error is just that the file/directory doesn't exist, it's fine.
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        errExit("remove-%s", SV_SOCK_PATH);
    }

    // Zero out the address, and set family and path.
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    // Bind the socket to the address. Note that we're binding the server socket
    // to a well-known address so that clients know where to connect.
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        errExit("bind");
    }

    // The listen call marks the socket as *passive*. The socket will subsequently
    // be used to accept connections from *active* sockets.
    // listen cannot be called on a connected socket (a socket on which a connect()
    // has been succesfully performed or a socket returned by a call to accept()).
    if (listen(sfd, BACKLOG) == -1) {
        errExit("listen");
    }

    ssize_t numRead;
    pid_t pid;
    char buf[BUF_SIZE];
    int children_pids[MAX_USERS];
    int pid_idx = 0;
    for (;;) {
        // Accept a connection. The connection is returned on a NEW
        // socket, 'cfd'; the listening socket ('sfd') remains open
        // and can be used to accept further connections. */
        printf("Waiting to accept a connection...\n");
        // NOTE: blocks until a connection request arrives.
        int cfd = accept(sfd, NULL, NULL);
        printf("Accepted socket fd = %d\n", cfd);

        // Create a separate process to handle a new client
        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit_gracefully(children_pids, sfd, EXIT_FAILURE, pid_idx);
        }
        else if (pid == 0) {
            // Transfer data from connected socket to stdout until EOF */
            int log_pid = getpid();
            printf("Log process PID: %d\n", log_pid);
            fflush(stdout);

            // Read at most BUF_SIZE bytes from the socket into buf.
            int out = STDOUT_FILENO;
            while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
                if (buf[0] == '@') {
                    // get the filename
                    char file[BUFFER_SIZE];
                    for (int i = 1; i < strlen(buf); i++)
                        file[i - 1] = buf[i];
                    file[strlen(buf) - 1] = '\0';

                    // concatenate the home path with a filename
                    char path[BUFFER_SIZE];
                    for(int i = 0; i < strlen(loggs_path); i++)
                        path[i] = loggs_path[i];
                    for(int i = strlen(loggs_path), j = 0; i < strlen(loggs_path) + strlen(file); i++, j++)
                        path[i] = file[j];
                    for(int i = strlen(loggs_path) + strlen(file), j = 0; i < strlen(loggs_path) + strlen(file) + 4; i++, j++)
                        path[i] = LOGGER_FORMAT[j];
                    path[strlen(loggs_path) + strlen(file) + 4] = '\0';

                    // Get file file descriptor.
                    int fd = open(path, O_WRONLY | O_CREAT);
                    if (fd > 0) out = fd;

                    // Create a file for the logger info.
                    strcat(log_fds_path, file);
                    strcat(log_fds_path, LOGGER_FORMAT);
                    // printf("ISL: %s", log_fds_path);
                    // Write logger info to file.
                    FILE* f = fopen(log_fds_path, "w");
                    fprintf(f, "%d\n%d", log_pid, fd);
                    fclose(f);
                    continue;
                }
                fflush(stdout);
                // Then, write those bytes from buf into STDOUT.
                if (write(out, buf, numRead) != numRead) {
                    fatal("partial/failed write");
                }
            }
            if (out > 0 && out != STDOUT_FILENO) close(out);

            if (numRead == -1) {
                errExit("read");
            }
        }
        children_pids[pid_idx++] = cfd;
    }
}


/* Function to exit the main server process.
 * It kills all children processes that were created by the server and it also closes the parent socket fd */
void exit_gracefully(int *children_pids, int socket_desc, int exit_code, int n_pids) {
    printf("\nServer shutting down...\n\n");
    for (int i = 0; i < n_pids; i++)
        kill(children_pids[i], SIGTERM);  // Kill all children :=(
    close(socket_desc);  // Close parent socket fd
    exit(exit_code);  // Goodbye!
}

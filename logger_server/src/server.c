#include "../inc/server.h"


int run_server(int argc, char *argv[]) {
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

        // Create a separate process to handle the new client
        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit_gracefully(children_pids, sfd, EXIT_FAILURE, pid_idx);
        }
        else if (pid == 0) {
            //
            // Transfer data from connected socket to stdout until EOF */
            //
            printf("Log process PID: %d\n", getpid());
            fflush(stdout);

            // Read at most BUF_SIZE bytes from the socket into buf.
            while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
                printf("INFO: ");
                fflush(stdout);
                // Then, write those bytes from buf into STDOUT.
                if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                    fatal("partial/failed write");
                }
            }

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

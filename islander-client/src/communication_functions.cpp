// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/communication_functions.h"
#include "../inc/defined_vars.h"
#include "../inc/wrappers.h"


void run_command_remotely(int sockfd, const std::string &buf, std::string &command_output_buf, bool need_encryption) {
    std::string cmd = buf + "\n";
    send_wrapper(need_encryption, sockfd, cmd);
    recv_cmd_output(sockfd, command_output_buf, need_encryption);
}


/** Function to receive comm output in simple or encrypted modes. Since output is sent splitted by packets,
 * so we need to take all of them until delimiter and combine  */
void recv_cmd_output(int client_socket_desc, std::string &command_output_buf, bool need_encryption) {
    ssize_t cc;
    char buf[SOCKET_BUFFER_SIZE];

    // Read from socket until COMMUNICATION_DELIMiTER appears
    size_t output_end;
    for (;;) {
        cc = recv_wrapper(need_encryption, client_socket_desc, buf);
        if (cc == 0) {
            // Executes if connection is closed gracefully. If closed unexpectedly, it won't be noticed.
            cout << "Close connection" << endl;
            close_sock_wrapper(need_encryption, client_socket_desc);
            exit(EXIT_SUCCESS);
        } else if (cc == -1) {
            continue;
        }
        buf[cc] = '\0';
        command_output_buf.append(buf);

        // Wait until COMMUNICATION_DELIMiTER appears (it might also appear many times)
        output_end = command_output_buf.find(COMMUNICATION_DELIMiTER);
        if (output_end != std::string::npos) {
            break;
        }
    }

    /* print all output except COMMUNICATION_DELIMiTER */
    if (command_output_buf != COMMUNICATION_DELIMiTER) {
        command_output_buf = command_output_buf.substr(0, output_end);
        cout << command_output_buf << endl;
    }
}

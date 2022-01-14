#ifndef MYSHELL_DEFINED_VARS_HPP
#define MYSHELL_DEFINED_VARS_HPP

#define SUCCESS              0
#define FUNC_FAILED         -1
#define OPEN_FILE_ERR       -2
#define READ_FILE_ERR       -3
#define WRITE_FILE_ERR      -4
#define ERROR_INPUT         -5
#define INVALID_ARG         -6

#define STDOUT_DESC          1
#define ERROR_DESC           2
//#define DEBUG_MODE

#define NOT_REDIRECT        "no"

#define MIN_PORT            1024
#define MAX_PORT            65535
#define NO_PORT             -1
#define DEFAULT_PORT        3000
#define SERVER_ADDRESS      "127.0.0.1"
#define SOCKET_BUF_SIZE     4092
#define COMMUNICATION_DELIMiTER  "COMMUNICATION_DELIMiTER"
#define FAIL    -1
#define OK    0
#define INVALID_CREDENTIALS    "\n################# Invalid credentials #################\n"

#define ENCRYPTED_PIPE_READ 0
#define ENCRYPTED_PIPE_WRITE 1

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)


#endif //MYSHELL_DEFINED_VARS_HPP

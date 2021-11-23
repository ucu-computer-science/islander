#ifndef MYSHELL_DEFINED_VARS_HPP
#define MYSHELL_DEFINED_VARS_HPP

#define STACKSIZE (1024 * 1024)
#define PIPE_READ 0
#define PIPE_WRITE 1
#define PIPE_FD_NUM 2
#define ROOT_UID 0
#define ROOT_GID 0
#define UID 1000
#define PIPE_OK_MSG "OK"
#define PIPE_MSG_SIZE 2

// error codes
#define SUCCESS 0
#define OPEN_FILE_ERR -2
#define READ_FILE_ERR -3
#define WRITE_FILE_ERR -4
#define ERROR_INPUT -5
#define INVALID_ARG -6
#define MOUNT_FAIL -7

#define STDOUT_DESC 1
#define ERROR_DESC 2
//#define DEBUG_MODE

#endif //MYSHELL_DEFINED_VARS_HPP

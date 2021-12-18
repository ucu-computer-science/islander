#ifndef CLIENT_SERVER_DEFINED_VARS_H
#define CLIENT_SERVER_DEFINED_VARS_H

#define SUCCESS 0
#define FUNC_FAILED -2

#define MIN_PORT            1024
#define MAX_PORT            65535
#define NO_PORT             -2
#define WRONG_UNIT          "-3"
#define TYPE_ERROR          "-4"
#define WRONG_ARGUMENTS     "-5"
#define DEFAULT_PORT        8080
#define SOCKET_BUF_SIZE     1024
#define SERVER_ADDRESS      "127.0.0.1"
#define SOCKET_BUFFER_SIZE     4092
#define FAIL    -1
#define COMMUNICATION_DELIMiTER  "COMMUNICATION_DELIMiTER"
#define INVALID_CREDS_MSG    "\n################# Invalid credentials #################\n"

// Default parameter values
#define DEFAULT_MEMORY_IN_BYTES         "500M"
#define DEFAULT_DEVICE_READ_BPS         "500M"
#define DEFAULT_DEVICE_WRITE_BPS        "100M"
#define DEFAULT_CPU_SHARES              "100"
#define DEFAULT_CPU_PERIOD              "100000"
#define DEFAULT_CPU_QUOTA               "1000000"

#endif //CLIENT_SERVER_DEFINED_VARS_H

// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/openssl_communication.h"
#include "../inc/communication_functions.h"

SSL *ssl;

void run_encrypted_client(int port, std::string& message, std::string& command_output_buf) {
    SSL_CTX *ctx;
    int server;

    SSL_library_init();
    ctx = InitCTX();
    server = OpenConnection(SERVER_ADDRESS, port);

    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, server);    /* attach the socket descriptor */
    if (SSL_connect(ssl) == FAIL)   /* perform the connection */
        ERR_print_errors_fp(stderr);
    else {
        if (is_authorized_user(server) != 0) {
            run_command_remotely(server, message, command_output_buf, true);
        }
    }
    SSL_free(ssl);         /* release connection state */
    close(server);         /* close socket */
    SSL_CTX_free(ctx);     /* release context */
}


int is_authorized_user(int server_fd) {
    char acClientRequest[1024] = {0};
    char ac_username[16] = "test_user"; // {0};
    char ac_password[16] = "123"; // {0};

    const char *cp_request_message = "UserName: %s, Password: %s\n";
//    cout << "To execute commands in encrypted mode, enter credentials...\n" << endl;
//    cout << "Enter the User Name : ";
//    scanf("%s", ac_username);
//    cout << "Enter the Password : ";
//    scanf("%s", ac_password);
    sprintf(acClientRequest, cp_request_message, ac_username,ac_password);   /* construct reply */
//    printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
//    ShowCerts(ssl);        /* get any certs */

    SSL_write(ssl, acClientRequest, strlen(acClientRequest));   /* encrypt & send message */

    std::string command_output_buf;
    recv_cmd_output(server_fd, command_output_buf, true);
    return command_output_buf != INVALID_CREDS_MSG;
}


int OpenConnection(const char *hostname, int port) {
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;
    if ( (host = gethostbyname(hostname)) == nullptr )
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}


SSL_CTX* InitCTX(void) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == nullptr )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}


void ShowCerts(SSL* ssl) {
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != nullptr ) {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), nullptr, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), nullptr, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

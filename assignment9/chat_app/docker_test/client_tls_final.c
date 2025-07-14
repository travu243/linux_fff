#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 1234

SSL *ssl;

void *recv_msg(void *arg) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = SSL_read(ssl, buffer, sizeof(buffer));
        if (len <= 0) break;
        printf("%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {

    //SSL_library_init registers TLS ciphers and digests
    SSL_library_init();
    //add all algorithms to table for lookup ciphers and digests
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    // object to establish TLS/SSL enabled connections
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024], input[1024], ip[100];

    printf("server ip: ");
    scanf("%s", ip);
    getchar();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // convert ip address from text to binary
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        return 1;
    }

    // object to hold the data for a TLS/SSL connection
    ssl = SSL_new(ctx);
    // set fd for in/output of ssl/tls
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sock);
        return 1;
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_msg, NULL);

    while (1) {
        fgets(input, sizeof(input), stdin);
        SSL_write(ssl, input, strlen(input));
    }

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

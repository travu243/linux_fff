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

int sock;
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
    struct sockaddr_in server_addr;
    char username[50], password[50], message[1024], ip[100];
    int mode, auth;

    // init openSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("unable to create SSL context");
        ERR_print_errors_fp(stderr);
        return 1;
    }

    printf("input ip address of server: ");
    scanf("%s", ip);
    getchar();  // clear newline

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("can not create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("invalid ip\n");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect fail");
        return 1;
    }

    // attach SSL to socket
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return 1;
    }

    printf("1. login\n2. sign in\n");
    scanf("%d", &mode);
    getchar();

    printf("username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    SSL_write(ssl, &mode, sizeof(int));
    SSL_write(ssl, username, sizeof(username));
    SSL_write(ssl, password, sizeof(password));

    SSL_read(ssl, &auth, sizeof(int));
    if (!auth) {
        printf("log in or sign in fail\n");
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return 0;
    }

    printf("log in successfull. accessing chat box...\n");

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_msg, NULL);

    while (1) {
        fgets(message, sizeof(message), stdin);
        SSL_write(ssl, message, strlen(message));
    }

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

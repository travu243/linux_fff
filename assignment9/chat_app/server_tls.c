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
#define MAX_CLIENTS 100

typedef struct {
    int sock;
    SSL *ssl;
    char username[50];
} client_t;

client_t *clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void save_chat_to_file(const char *username, const char *message) {
    FILE *fp = fopen("chatlog.txt", "a");
    if (fp) {
        fprintf(fp, "%s: %s", username, message);
        fclose(fp);
    }
}

void broadcast(const char *message, client_t *sender) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] != sender) {
            SSL_write(clients[i]->ssl, message, strlen(message));
        }
    }
    pthread_mutex_unlock(&lock);
}

void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    char buffer[1024], full_msg[1080];
    int len;

    // send chat log
    FILE *fp = fopen("chatlog.txt", "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            SSL_write(cli->ssl, buffer, strlen(buffer));
        }
        fclose(fp);
    }

    while ((len = SSL_read(cli->ssl, buffer, sizeof(buffer))) > 0) {
        buffer[len] = 0;
        snprintf(full_msg, sizeof(full_msg), "%s: %s", cli->username, buffer);
        save_chat_to_file(cli->username, buffer);
        broadcast(full_msg, cli);
    }

    close(cli->sock);
    SSL_shutdown(cli->ssl);
    SSL_free(cli->ssl);

    pthread_mutex_lock(&lock);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == cli) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    free(cli);
    pthread_exit(NULL);
}

int main() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);

    printf("TLS chat server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sock);
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            close(client_sock);
            SSL_free(ssl);
            continue;
        }

        int mode, auth = 1;
        char username[50], password[50];

        SSL_read(ssl, &mode, sizeof(int));
        SSL_read(ssl, username, sizeof(username));
        SSL_read(ssl, password, sizeof(password));
        // acp all connection
        SSL_write(ssl, &auth, sizeof(int));

        client_t *cli = malloc(sizeof(client_t));
        cli->sock = client_sock;
        cli->ssl = ssl;
        strncpy(cli->username, username, sizeof(cli->username));

        pthread_mutex_lock(&lock);
        clients[client_count++] = cli;
        pthread_mutex_unlock(&lock);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, cli);
        pthread_detach(tid);
    }

    close(server_sock);
    SSL_CTX_free(ctx);
    return 0;
}

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
    SSL *ssl;
    int active;
    char username[50];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// broadcast to all client
void broadcast(const char *msg, SSL *sender) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].ssl != sender) {
            SSL_write(clients[i].ssl, msg, strlen(msg));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void save_chat_log(const char *message) {
    FILE *log = fopen("chatlog.txt", "a");
    if (log) {
        fputs(message, log);
        fclose(log);
    }
}

void send_chat_log(SSL *ssl) {
    FILE *log = fopen("chatlog.txt", "r");
    if (log) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), log)) {
            SSL_write(ssl, buffer, strlen(buffer));
        }
        fclose(log);
    }
}

// init 1 client
int add_client(SSL *ssl, const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].ssl = ssl;
            clients[i].active = 1;
            strncpy(clients[i].username, username, sizeof(clients[i].username));
            pthread_mutex_unlock(&clients_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

// deinit 1 client
void remove_client(SSL *ssl) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].ssl == ssl) {
            clients[i].active = 0;
            clients[i].ssl = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int authenticate(SSL *ssl, char *username_out) {
    char buffer[1024], username[50], password[50];
    int mode = 0;
    int auth = 0;

    do{
        SSL_write(ssl, "1. login\n2. sign in\n", strlen("1. login\n2. sign in\n"));
        memset(buffer, 0, sizeof(buffer));
        SSL_read(ssl, buffer, sizeof(buffer));
        mode = atoi(buffer);
        if(mode!=1 && mode!=2) SSL_write(ssl, "try again\n", strlen("try again\n"));
    }while(mode!=1 && mode!=2);

    SSL_write(ssl, "Username: ", strlen("Username: "));
    SSL_read(ssl, username, sizeof(username));
    username[strcspn(username, "\n")] = 0;

    SSL_write(ssl, "Password: ", strlen("Password: "));
    SSL_read(ssl, password, sizeof(password));
    password[strcspn(password, "\n")] = 0;

    FILE *file = fopen("users.txt", "a+");
    if (!file) return 0;

    char line[100];
    if (mode == 1) { // login
        while (fgets(line, sizeof(line), file)) {
            char file_user[50], file_pass[50];
            sscanf(line, "%s %s", file_user, file_pass);
            if (strcmp(file_user, username) == 0 && strcmp(file_pass, password) == 0) {
                auth = 1;
                break;
            }
        }
    }
    else if (mode == 2) { // sign in
        int exists = 0;
        while (fgets(line, sizeof(line), file)) {
            char file_user[50];
            sscanf(line, "%s", file_user);
            // check if have same username
            if (strcmp(file_user, username) == 0) {
                exists = 1;
                break;
            }
        }
        if (!exists) {
            fprintf(file, "%s %s\n", username, password);
            fflush(file);
            auth = 1;
        }
    }

    fclose(file);

    sprintf(buffer, "%d", auth);
    SSL_write(ssl, buffer, strlen(buffer));

    if (auth)
        strncpy(username_out, username, 50);

    return auth;
}

void *handle_client(void *arg) {
    SSL *ssl = (SSL *)arg;
    char buffer[1024];
    char username[50];

    if (!authenticate(ssl, username)) {
        SSL_write(ssl, "Authentication failed. Closing connection.\n", 44);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        pthread_exit(NULL);
    }

    SSL_write(ssl, "Login successful. Welcome\n", 39);
    printf("%s joined the chat\n", username);

    add_client(ssl, username);

    char join_msg[150];
    snprintf(join_msg, sizeof(join_msg), "[SERVER]: %s joined the chat\n", username);
    broadcast(join_msg, NULL);

    save_chat_log(join_msg);
    send_chat_log(ssl);

    // chat
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = SSL_read(ssl, buffer, sizeof(buffer));
        if (len <= 0) break;

        char msg[1100];
        snprintf(msg, sizeof(msg), "%s: %s", username, buffer);
        broadcast(msg, ssl);
        save_chat_log(msg);
    }

    printf("%s disconnected\n", username);

    char leave_msg[150];
    snprintf(leave_msg, sizeof(leave_msg), "[SERVER]: %s left the chat\n", username);
    broadcast(leave_msg, NULL);
    save_chat_log(leave_msg);

    remove_client(ssl);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    pthread_exit(NULL);
}

int main() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    if (!SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) ||
        !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) ||
        !SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);

    printf("TLS chat server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len);

        // ssl set fd for client sock
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sock);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            close(client_sock);
            SSL_free(ssl);
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, ssl);
        pthread_detach(tid);
    }

    close(server_sock);
    SSL_CTX_free(ctx);
    return 0;
}

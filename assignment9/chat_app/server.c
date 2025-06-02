#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define MAX_CLIENTS 100

typedef struct {
    int sock;
    char username[50];
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

void save_chat_to_file(const char *username, const char *message) {
    pthread_mutex_lock(&file_mutex);
    FILE *fp = fopen("chatlog.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "%s: %s", username, message);
        fclose(fp);
    }
    pthread_mutex_unlock(&file_mutex);
}

void broadcast(char *message, client_t *sender) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].sock != sender->sock) {
            send(clients[i].sock, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    char buffer[1024], full_msg[1024];
    int len;

    //send log chat for new client
    pthread_mutex_lock(&file_mutex);
    FILE *fp = fopen("chatlog.txt", "r");
    if (fp != NULL) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            send(cli->sock, buffer, strlen(buffer), 0);
        }
        fclose(fp);
    }
    pthread_mutex_unlock(&file_mutex);

    //enter chat room
    snprintf(full_msg, sizeof(full_msg), "[SERVER]: %s has joined\n", cli->username);
    broadcast(full_msg, cli);

    //receive messages, save mess to chat log and send to all clients
    while ((len = recv(cli->sock, buffer, sizeof(buffer), 0)) > 0) {
        buffer[len] = 0;
        snprintf(full_msg, sizeof(full_msg), "%s: %s", cli->username, buffer);
        save_chat_to_file(cli->username, buffer);
        broadcast(full_msg, cli);
    }

    //exit chat room
    snprintf(full_msg, sizeof(full_msg), "[SERVER]: %s has leaved\n", cli->username);
    broadcast(full_msg, cli);

    // disconnect
    close(cli->sock);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].sock == cli->sock) {
            clients[i] = clients[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    free(cli);
    return NULL;
}

int check_user(const char *username, const char *password) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return 0;

    char u[50], p[50];
    while (fscanf(fp, "%s %s", u, p) != EOF) {
        if (strcmp(username, u) == 0 && strcmp(password, p) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int register_user(const char *username, const char *password) {
    FILE *fp = fopen("users.txt", "a");
    if (!fp) return 0;
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
    return 1;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("can not create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bind ip address to socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind fail");
        return 1;
    }

    listen(server_sock, 10);
    printf("server is lisening on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);

        int mode;
        char username[50], password[50];

        recv(client_sock, &mode, sizeof(int), 0);
        recv(client_sock, username, sizeof(username), 0);
        recv(client_sock, password, sizeof(password), 0);
        int auth = 0;
        if (mode == 1) {
            auth = check_user(username, password);
        } else if (mode == 2) {
            auth = register_user(username, password);
        }

        send(client_sock, &auth, sizeof(int), 0);
        if (!auth) {
            close(client_sock);
            continue;
        }

        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->sock = client_sock;
        strncpy(cli->username, username, sizeof(cli->username));

        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = *cli;
            pthread_t tid;
            pthread_create(&tid, NULL, handle_client, (void *)cli);
        } else {
            close(client_sock);
            free(cli);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    close(server_sock);
    return 0;
}

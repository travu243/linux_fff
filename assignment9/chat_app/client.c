#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234

int sock;

void *recv_msg(void *arg) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(sock, buffer, sizeof(buffer), 0);
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

    printf("input ip address of server: ");
    scanf("%s", ip);
    getchar();  // clear newline from buffer

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

    printf("1. login\n2. sign in\n");
    scanf("%d", &mode);
    getchar();

    printf("username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;


    send(sock, &mode, sizeof(int), 0);
    send(sock, username, sizeof(username), 0);
    send(sock, password, sizeof(password), 0);

    recv(sock, &auth, sizeof(int), 0);
    if (!auth) {
        printf("log in or sign in fail\n");
        close(sock);
        return 0;
    }

    printf("log in successfull. accessing chat box...\n");

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_msg, NULL);

    while (1) {
        fgets(message, sizeof(message), stdin);
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>

#define MAX_EVENTS 10

int main() {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // create pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // write to pipe
    const char *initial_msg = "welcome\n";
    write(pipefd[1], initial_msg, strlen(initial_msg));

    // sign up stdin
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl: stdin");
        exit(EXIT_FAILURE);
    }

    // sign up read pipe
    ev.events = EPOLLIN;
    ev.data.fd = pipefd[0];
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &ev) == -1) {
        perror("epoll_ctl: pipefd[0]");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];

    printf("wait for input on stdin or pipe...\n");

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == STDIN_FILENO) {
                char buf[1024];
                ssize_t count = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                if (count > 0) {
                    buf[count] = '\0';
                    printf("stdin: %s\n", buf);

                    // write to pipe
                    //write(pipefd[1], buf, count);
                    write(pipefd[1],"hello\n", 5);
                }
            } else if (fd == pipefd[0]) {
                char buf[1024];
                ssize_t count = read(pipefd[0], buf, sizeof(buf) - 1);
                if (count > 0) {
                    buf[count] = '\0';
                    printf("pipe: %s\n", buf);
                }
            }
        }
    }

    close(epoll_fd);
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}

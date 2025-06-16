#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

int main() {
    fd_set read_fds;
    struct timeval timeout;
    int retval;

    // timeout 10 seconds
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // set file descriptor set
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds); // STDIN_FILENO is 0

    printf("Wait input\n");

    retval = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

    if (retval == -1) {
        perror("select()");
        return 1;
    }
      else if (retval == 0) {
        printf("timeout. no input\n");
      }
      else {
        char buffer[100];

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                printf("%s", buffer);
            }
        }
    }
    return 0;
}



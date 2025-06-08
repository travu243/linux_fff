#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void say_hello();
void say_goodbye();
void handle_signal(int sig);

int running = 1;

int main() {
    // signal handler
    signal(SIGINT, handle_signal);

    // functions pointer
    void (*functions[])() = { say_hello, say_goodbye };
    int choice;

    while (running) {
        printf("\nmenu:\n");
        printf("1. say hello\n");
        printf("2. say hoodbye\n");
        printf("0. exit\n");
        printf("enter choice: ");
        scanf("%d", &choice);

        if (choice == 0) {
            break;
        } else if (choice >= 1 && choice <= 2) {
            functions[choice - 1]();  // call function via pointer
        } else {
            printf("invalid choice!\n");
        }

        sleep(1);
    }

    printf("program exited normally.\n");
    return 0;
}

void say_hello() {
    printf("hello\n");
}

void say_goodbye() {
    printf("goodbye\n");
}

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\ncaught SIGINT. exit\n");
        running = 0;
    }
}

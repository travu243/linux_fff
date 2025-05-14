#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

// List of common signals to check
int signals_to_check[] = {
    SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGKILL,
    SIGSEGV, SIGPIPE, SIGALRM, SIGTERM, SIGUSR1, SIGUSR2,
    SIGCHLD, SIGCONT, SIGSTOP, SIGTSTP, SIGTTIN, SIGTTOU
};

const char* signal_name(int sig) {
    switch (sig) {
        case SIGHUP: return "SIGHUP";
        case SIGINT: return "SIGINT";
        case SIGQUIT: return "SIGQUIT";
        case SIGILL: return "SIGILL";
        case SIGABRT: return "SIGABRT";
        case SIGFPE: return "SIGFPE";
        case SIGKILL: return "SIGKILL";
        case SIGSEGV: return "SIGSEGV";
        case SIGPIPE: return "SIGPIPE";
        case SIGALRM: return "SIGALRM";
        case SIGTERM: return "SIGTERM";
        case SIGUSR1: return "SIGUSR1";
        case SIGUSR2: return "SIGUSR2";
        case SIGCHLD: return "SIGCHLD";
        case SIGCONT: return "SIGCONT";
        case SIGSTOP: return "SIGSTOP";
        case SIGTSTP: return "SIGTSTP";
        case SIGTTIN: return "SIGTTIN";
        case SIGTTOU: return "SIGTTOU";
        default: return "UNKNOWN";
    }
}

int main() {
    sigset_t curr_mask;

     sigset_t set;
    sigemptyset(&set); // Tạo một signal set rỗng
    sigaddset(&set, SIGINT); // Thêm SIGINT vào set

    // Chặn tín hiệu SIGINT
    sigprocmask(SIG_BLOCK, &set, NULL);

    // Get current signal mask
    if (sigprocmask(SIG_SETMASK, NULL, &curr_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("Currently blocked signals:\n");

    for (size_t i = 0; i < sizeof(signals_to_check)/sizeof(signals_to_check[0]); ++i) {
        int sig = signals_to_check[i];
        if (sigismember(&curr_mask, sig)) {
            printf(" - %s\n", signal_name(sig));
        }
    }

    return 0;
}

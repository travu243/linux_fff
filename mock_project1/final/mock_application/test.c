#include <stdio.h>// printf(), perror(), snprintf()
#include <fcntl.h>// open(), O_RDWR, O_RDONLY
#include <unistd.h>// close(), sleep(), read(), write()
#include <stdlib.h>// exit(), malloc(), free()
#include <termios.h>// struct termios, tcgetattr(), tcsetattr()
#include <string.h>// strlen(), memset()...
#include <pthread.h>// pthread_create(), pthread_join()
#include <stdbool.h>// `bool`, `true`, `false`
#include <sys/ioctl.h>// ioctl(), _IOW, _IOR macro

#define DEVICE "/dev/ledled"
#define I2C_DEV "/dev/i2c_ledchar"

#define LEDBTN_SET _IOW(99, 0, int)
#define LEDBTN_GET _IOR(99, 1, int)

#define I2C_GET_LAST_VALUE _IOR(0x99, 0, int)


static volatile bool running = true;
static int fd;
static int i2c_fd;

// show led status every second
void *led_status_thread(void *arg) {
    while (running) {
        int val;
        if (ioctl(fd, LEDBTN_GET, &val) == 0) {

            printf("\033[5;0Hled status: %s    \n", val ? "on " : "off");

            printf("\033[8;0H"); //cursor line 8
            printf("=== LED CONTROL ===\n");
            printf("1 turn on led\n");
            printf("2 turn off led\n");
            printf("q quit\n\n");

        } else {
            perror("ioctl(LEDBTN_GET)");
        }
        sleep(1);
    }
    return NULL;
}

// show i2c value every second


void *i2c_value_thread(void *arg) {
    while (running) {
        int val;
        if (ioctl(i2c_fd, I2C_GET_LAST_VALUE, &val) == 0)
            printf("\033[6;0Hi2c led: %d   \n", val);

        else
            perror("ioctl(I2C_GET_LAST_VALUE)");

        sleep(1);
    }
    return NULL;
}


// read char without enter
char getch() {
    struct termios oldt, newt;
    char c;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}

void print_menu() {
    printf("\033[2J"); // clear screen
    printf("\033[8;0H"); //cursor line 8
    printf("=== LED CONTROL ===\n");
    printf("1 turn on led\n");
    printf("2 turn off led\n");
    printf("q quit\n\n");
}

int main() {
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open /dev/ledled");
        return 1;
    }

    i2c_fd = open(I2C_DEV, O_RDONLY);
    if (i2c_fd < 0) {
        perror("open /dev/i2c_ledchar");
        close(fd);
        return 1;
    }

    pthread_t thread_led, thread_i2c;
    pthread_create(&thread_led, NULL, led_status_thread, NULL);
    pthread_create(&thread_i2c, NULL, i2c_value_thread, NULL);

    print_menu();

    while (1) {
        char c = getch();
        int val;

        if (c == '1') {
            val = 1;
            ioctl(fd, LEDBTN_SET, &val);
        } else if (c == '2') {
            val = 0;
            ioctl(fd, LEDBTN_SET, &val);
        } else if (c == 'q' || c == 'Q') {
            running = false;
            break;
        }
    }

    pthread_join(thread_led, NULL);
    pthread_join(thread_i2c, NULL);
    close(fd);
    close(i2c_fd);

    printf("\033[2J");
    printf("bye bye\n");
    return 0;
}

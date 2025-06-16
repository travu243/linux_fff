#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define IOCTL_NUMBER 0xF5
#define IOCTL_CLEAR_BUFFER _IO( IOCTL_NUMBER, 0)
#define IOCTL_GET_SIZE     _IOR( IOCTL_NUMBER, 1, int)

int main() {
    int fd = open("/dev/my_device", O_RDWR);
    if (fd < 0) return perror("open"), 1;

    int size = 0;
    ioctl(fd, IOCTL_GET_SIZE, &size);
    printf("buffer size: %d\n", size);

    ioctl(fd, IOCTL_CLEAR_BUFFER);

    ioctl(fd, IOCTL_GET_SIZE, &size);
    printf("buffer size: %d\n", size);

    close(fd);
    return 0;
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define LED_ON = 1
#define LED_OFF = 0

main()
{
    int fd = 0;
    int ledno = 0;
    int i = 0;

    fd = open("/dev/fs4412",O_RDWR);
    if(fd < 0)
        perror("open failed");


    while(1)
    {
        ioctl(fd,1);
         sleep(2);
        ioctl(fd,0);
        sleep(2);
    }

    close(fd);
}

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
#if 0
        for(;i<1000;i++)
        {
            ledno = i%4;
            ioctl(fd,LED_ON,ledno);
            usleep(10000);
            ioctl(fd,LED_OFF,ledno);
            usleep(10000);
        }
#endif

    while(1)
    {
        ioctl(fd,1);
         sleep(5);
        ioctl(fd,0);
        sleep(5);
    }

    close(fd);
}

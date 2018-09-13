#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>


void fs4412_beep_release(struct device *dev)
{
    printk("fs4412-beep_release");
}


/*
*struct platform_device {
	*const char	*name;
	*int		id;
	bool		id_auto;
	*struct device	dev;
    *u32		num_resources;
	*struct resource	*resource; 存放硬件的寄存器信息

	const struct platform_device_id	*id_entry;

*/
struct resource beep_res[] = {
    [0] = {
        .start = 0x114000a0, //GPDOCON
        .end = 0x114000a0+3,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 0x139d0000, //TIMER0
        .end = 0x139d0000+0x13,
        .flags = IORESOURCE_MEM,
    },
};

struct platform_device platform_beep ={

    .name = "fs4412-beep",
    .id = -1,
    .dev.release = fs4412_beep_release,
    .resource = beep_res,
    .num_resources = ARRAY_SIZE(beep_res),


};

static int __init platform_beep_init(void)
{
    printk("your first code in Linux\n");
    platform_device_register(&platform_beep);//设备注册
    return 0;
}

static void __exit platform_beep_exit(void)
{
    printk("platform_beep module exit\n ");
    platform_device_unregister(&platform_beep);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("FS");
MODULE_VERSION("2.0");
module_init(platform_beep_init);
module_exit(platform_beep_exit);
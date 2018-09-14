#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioport.h>

#define	TCFG0 0x0
#define TCFG1 0x4
#define TCON 0x8
#define TCNTB0 0xc
#define TCMPB0 0x10

dev_t led_no = 0;

int major = 0;
int minor = 0;

struct class *pcls;
struct device *plediv;

void *pgpd0con;
void *ptimer0base;
int fs4412_beep_ioremap(struct platform_device *pdev)
{
    pgpd0con = ioremap(pdev->resource[0].start,4);
    ptimer0base = ioremap(pdev->resource[1].start,0x14);

    return 0;
}

int fs4412_beep_iounmap(void)
{
    iounmap( pgpd0con);
    iounmap(ptimer0base);

    return 0;
}


int fs4412_beep_init(void)
{
    writel((readl(pgpd0con)& ~(0xf))| 0x2,pgpd0con);
    writel((readl(ptimer0base+TCFG0) & ~(0xff))| 0xff,ptimer0base+TCFG0); 
    writel((readl(ptimer0base+TCFG1) & ~(0xf))| 0x1,ptimer0base+TCFG1); 
    writel(100,ptimer0base+TCNTB0);
    writel(50,ptimer0base+TCMPB0);
    writel((readl(ptimer0base+TCON) & ~(0xf)) | 1<<1 ,ptimer0base+TCON);

    printk("init success\n");

  

    return 0;
}
//open) (struct inode *, struct file *);
int fs4412_open(struct inode *inode, struct file *file)
{
    printk("open\n");
    return 0;
  //  fs4412_led_on(ledno);
}

int fs4412_close(struct inode *inode, struct file *file)
{
    printk("close\n");
    return 0;
  //  fs4412_led_off(ledno);
}

int fs4412_beep_on(void)
{
    writel((readl(ptimer0base+TCON) & ~(0xff))| 1<<0 | 1<<3 ,ptimer0base+TCON);
    return 0;
}

int fs4412_beep_off(void)
{
    writel(readl(ptimer0base+TCON) & ~(1<<0) ,ptimer0base+TCON);
    return 0;
}


long fs4412_beep_ioctl(struct file *file, unsigned int cmd, unsigned long fre)
{
    printk("the cmd is %d\n",cmd);
    switch(cmd)
    {
        case 1: 
            fs4412_beep_on();
            break;
        case 0:
            fs4412_beep_off();
            break;
        // case 2:
        //     fs4412_beep_setfrequency(fre);
        //     break;
        default:
            printk("error\n");
    }

    return 0;

}

struct of_device_id beep_tbl[] = {
    {.compatible = "fs4412,beep"},
};


struct file_operations fops = 
{
    .open = fs4412_open, 
    .release = fs4412_close,
   // .unlocked_ioctl = fs4412_led_ioctl,
    .unlocked_ioctl  = fs4412_beep_ioctl,
};


//在probe接口中实现驱动的初始化操作
int platform_beep_probe(struct platform_device *pdev)  //探测函数接口
{
    printk("platform beep probe success \n");
    major = register_chrdev(major,"fs4412",&fops);
    printk("major from register_chardev:%d\n",major);

   
    led_no = MKDEV(major,minor);
    pcls = class_create(THIS_MODULE,"class");
    plediv = device_create(pcls,NULL,led_no,NULL,"fs4412");


    fs4412_beep_ioremap(pdev);

    fs4412_beep_init();
    return 0;
}

int platform_beep_remove(struct platform_device *pdev)
{
    printk("platform beep remove\n");
     unregister_chrdev(major,"fs4412");
    device_destroy(pcls,led_no);
    class_destroy(pcls);
//    fs4412_led_iounmap();
    fs4412_beep_iounmap();

    return 0;
}

int platform_beep_suspend(struct platform_device *pdev,pm_message_t state) //低功耗状态
{
    printk("platform beep suspend");
    return 0;
}

int platform_beep_resume(struct platform_device *pdev)
{
    printk("platform beep resume\n");
    return 0;
}

struct platform_driver platform_beep_driver = {

// struct platform_driver {
// 	int (*probe)(struct platform_device *);
// 	int (*remove)(struct platform_device *);
// 	void (*shutdown)(struct platform_device *);
// 	int (*suspend)(struct platform_device *, pm_message_t state);
// 	int (*resume)(struct platform_device *);
// 	struct device_driver driver;
// 	const struct platform_device_id *id_table;
// 	bool prevent_deferred_probe;
// };
    .driver.name = "fs4412-beep", //这个和设备中定义的名字相同才能匹配到-》探测
    .remove = platform_beep_remove,
    .suspend = platform_beep_suspend,
    .resume = platform_beep_resume, //复用功能
    .probe = platform_beep_probe,
    .driver.of_match_table = beep_tbl,


};


static int __init platform_beep_driver_init(void)
{
    printk("platform_driver init \n");
    platform_driver_register(&platform_beep_driver);
    return 0;
}

static void __init platform_beep_driver_exit(void)
{
    printk("platform_driver exit \n");
    platform_driver_unregister(&platform_beep_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FS");
MODULE_VERSION("2.0");
module_init(platform_beep_driver_init);
module_exit(platform_beep_driver_exit);
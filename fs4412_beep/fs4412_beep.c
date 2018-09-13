#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/delay.h>

//�������оƬ������Ĵ���
#define GPX2CON 0x11000c40
#define GPX2DAT 0x11000c44
#define GPX1CON 0x11000c20
#define GPX1DAT 0x11000c24
#define GPF3CON 0x114001e0
#define GPF3DAT 0x114001e4
//���������
#define GPD0CON 0x114000a0 
#define	TCFG0 0x139d0000
#define TCFG1 0x139d0004
#define TCON 0x139d0008
#define TCNTB0 0x139d000c
#define TCMPB0 0x139d0010
#define GPD0PUD 0x114000a8

#define LED_ON = 1;
#define LED_OFF = 0;


dev_t led_no = 0;//�豸�ţ���12λ�����豸�ţ���20λ�Ǵ��豸�ţ�
//int major = 188;//���豸�ţ����紮��������
int major = 0;
int minor = 0;//���豸�ţ�����ڼ������ڣ�

struct class *pcls;
struct device *plediv;


//�ں˲���ֱ�ӷ��������ַ��Ҫ��ӳ��������ַ
void *pgpx2con;
void *pgpx2dat;
void *pgpx1con;
void *pgpx1dat;
void *pgpf3con;
void *pgpf3dat;
//���������
void *pgpd0con;
void *ptcfg0;
void *ptcfg1;
void *ptcon;
void *ptcntb0;
void *ptcmpb0;
void *pgpd0pud;


int fs4412_beep_ioremap(void)
{
    pgpd0con = ioremap(GPD0CON,4);
    ptcfg0 = ioremap(TCFG0,4);
    ptcfg1 = ioremap(TCFG1,4);
    ptcon = ioremap(TCON,4);
    ptcntb0 = ioremap(TCNTB0,4);
    ptcmpb0 = ioremap(TCMPB0,4);
    pgpd0pud = ioremap(GPD0PUD,4);

    return 0;
}

// int fs4412_led_ioremap(void)
// {
//     pgpx2con = ioremap(GPX2CON,4);//�����ַ���ֽ���
//     pgpx2dat = ioremap(GPX2DAT,4);
//     pgpx1con = ioremap(GPX1CON,4);//�����ַ���ֽ���
//     pgpx1dat = ioremap(GPX1DAT,4);
//     pgpf3con = ioremap(GPF3CON,4);//�����ַ���ֽ���
//     pgpf3dat = ioremap(GPF3DAT,4);

//     return 0;
// }
//���ӳ��
// int fs4412_led_iounmap(void)
// {
//     iounmap(pgpx2con);
//     iounmap(pgpx2dat);
//     iounmap(pgpx1con);
//     iounmap(pgpx1dat);
//     iounmap(pgpf3con);
//     iounmap(pgpf3dat);
    

//     return 0;
// }

int fs4412_beep_iounmap(void)
{
    iounmap(pgpd0con);
    iounmap(ptcfg0);
    iounmap(ptcon);
    iounmap(ptcfg1);
    iounmap(ptcntb0);
    iounmap(ptcmpb0);
    iounmap(pgpd0pud);

    return 0;
}

int fs4412_beep_init(void)
{
    writel((readl(pgpd0con)& ~(0xf))| 0x2,pgpd0con);//TOUT0
    writel(readl(pgpd0pud)& ~(0xf),pgpd0pud);
    writel((readl(ptcfg0) & ~(0xff))| 0xff,ptcfg0); //Ԥ��Ƶ
    writel((readl(ptcfg1) & ~(0xf))| 0x1,ptcfg1); //��Ƶ
    writel(100,ptcntb0);
    writel(50,ptcmpb0);
    writel((readl(ptcon) & ~(0xf)) | 1<<1 ,ptcon);//����ʱ��0�����¼�����

    printk("init success\n");

  

    return 0;
}
//GPIO ��ʼ��->����gpx2_7�����ģʽ
// int fs4412_led_init(void)
// {
//     writel((readl(pgpx2con)&~(0xf<<28))|0x1<<28,pgpx2con);//д���������ĵ�ַ
//     writel((readl(pgpx1con)&~(0xf))|0x1,pgpx1con);
//     writel((readl(pgpf3con)&~(0xf<<20))|0x1<<20,pgpf3con);
//     writel((readl(pgpf3con)&~(0xf<<16))|0x1<<16,pgpf3con);
    
//     return 0;

// }


int fs4412_beep_on(void)
{
    writel((readl(ptcon) & ~(0xf))| 1<<0 | 1<<3 ,ptcon);
    printk("success beep on\n");
    return 0;
}

int fs4412_beep_off(void)
{
    writel((readl(ptcon) & ~(0x1)),ptcon);
    printk("success beep off\n");
    return 0;
}

// int fs4412_led_on(int ledno)
// {
//     printk("fs4412_led_open\n");
//     //����gpio����ߵ�ƽ����led
//     switch(ledno)
//     {
//         case 0:
//             writel(readl(pgpx2dat) | (0x1<<7),pgpx2dat);
//             break;
//         case 1:
//             writel(readl(pgpx1dat) | (0x1<<0),pgpx1dat);
//             break;
//         case 2:
//             writel(readl(pgpf3dat) | (0x1<<4),pgpf3dat);
//             break;
//         case 3:
//             writel(readl(pgpf3dat) | (0x1<<5),pgpf3dat);
//             break;
//         default:
//             printk("error\n");

//     }

//     return 0;
// }

// int fs4412_led_off(int ledno)
// {
//     printk("fs4412_led_close\n");
//     //����gpioϨ��led
//     switch(ledno)
//     {
//         case 0:
//             writel(readl(pgpx2dat) & ~(0x1<<7),pgpx2dat);
//             break;
//         case 1:
//             writel(readl(pgpx1dat) & ~(0x1<<0),pgpx1dat);
//             break;
//         case 2:
//             writel(readl(pgpf3dat) & ~(0x1<<4),pgpf3dat);
//             break;
//         case 3:
//             writel(readl(pgpf3dat) & ~(0x1<<5),pgpf3dat);  
//             break;
//         default:
//             printk("error\n");



//     }

//     return 0;
// }

long fs4412_beep_ioctl(struct file *file, unsigned int cmd, unsigned long ledno)
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
        default:
            printk("error\n");
    }

    return 0;

}
#if 0
long fs4412_led_ioctl(struct file *file, unsigned int cmd, unsigned long ledno)
{
    printk("ledno is %d\n",ledno);
    switch(cmd)
    {
        case 1: 
            fs4412_led_on(ledno);
            break;
        case 0:
            fs4412_led_off(ledno);
            break;
        default:
            printk("error\n");
    }

    return 0;

}
#endif

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




//struct cdev led_cdev;//�豸��Ϣ
//�豸����Ϊ����
//long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
struct file_operations fops = 
{
    .open = fs4412_open, //������ֵ������ʵ����Ҳ�Ǿ��е�ַ���ڴ棩
    .release = fs4412_close,
   // .unlocked_ioctl = fs4412_led_ioctl,
    .unlocked_ioctl  = fs4412_beep_ioctl,
};


//**********************************************************************************
static int  __init fs4412_init(void)
{
#if 0
    //��̬�����豸��
    // register_chrdev_region(led_no,1,"static-led_no");
    alloc_chrdev_region(&led_no,0,1,"fs4412-led");//��̬����
	
	printk("register a dev in Linux\n");

    //��ʼ��cdev���豸����Ϣ��
    cdev_init(&led_cdev,&fops); //��fops�������Ϊ���������豸��Ϣ�Ľṹ��
    cdev_add(&led_cdev,led_no,1);//���ַ��豸��ӵ�ϵͳ��
#endif

    //�ڶ��ֳ�ʼ���ӿڣ��൱�ڷ�װ��cdev,cdev_init,��cdev_add������̬�����豸�ţ�
    major = register_chrdev(major,"fs4412",&fops);//major Ҫ=0 ���ܶ�̬�����豸��
    printk("major from register_chardev:%d\n",major);

    //�Զ������豸�ڵ�
    led_no = MKDEV(major,minor);
    pcls = class_create(THIS_MODULE,"class");
    plediv = device_create(pcls,NULL,led_no,NULL,"fs4412");

    //��ʼ��Ӳ�������ӿ�
  //  fs4412_led_ioremap();
    fs4412_beep_ioremap();

    fs4412_beep_init();
  //  fs4412_led_init();

  




	return 0;
}

static void __exit fs4412_exit(void)
{
	printk("fs4412 module exit\n");
#if 0 //��һ�ų�ʼ�����ͷŷ�ʽ
    //�ͷ���Դ
    unregister_chrdev_region(led_no,1);//���豸��
    cdev_del(&led_cdev);//���cdev�Ľṹ��
#endif

#if 1
    //�ڶ����ͷŵķ�ʽ
    unregister_chrdev(major,"fs4412");
    device_destroy(pcls,led_no);
    class_destroy(pcls);
//    fs4412_led_iounmap();
    fs4412_beep_iounmap();

#endif

}
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fs");
MODULE_VERSION("1.0");
module_init(fs4412_init);
module_exit(fs4412_exit);

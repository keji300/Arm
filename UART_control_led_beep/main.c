#include "exynos_4412.h"

int  mystrcopy(char*,char*);

int flag;
void led_init()
{
	//gpx2_7    [31:28]
	GPX2.CON = (GPX2.CON &(~(0xf<<28))) | (0x1<<28) ;
	//gpx1_0    [3:0]
	GPX1.CON = (GPX1.CON &(~(0xf<<0))) | (0x1<<0) ;
	//GPF3-4 -5  [23:20][ 19:16]
	GPF3.CON = (GPX1.CON &(~(0xff<<16))) | (0x11<<16) ;
}
led_on(int ledno)
{
	switch(ledno)
	{
		case 1:
			GPX2.DAT = GPX2.DAT | 0x1<<7;
			break;
		case 2:
			GPX1.DAT = GPX1.DAT | 0x1<<0;
			break;
		case 3:
			GPF3.DAT = GPF3.DAT | 0x1<<4;
			break;
		case 4:
			GPF3.DAT = GPF3.DAT | 0x1<<5;
			break;
	}
}
led_off(int ledno)
{
	switch(ledno)
	{
		case 1:
			GPX2.DAT = GPX2.DAT & (~(0x1<<7));
			break;
		case 2:
			GPX1.DAT = GPX1.DAT & (~(0x1<<0));
			break;
		case 3:
			GPF3.DAT = GPF3.DAT & (~(0x1<<4));
			break;
		case 4:
			GPF3.DAT = GPF3.DAT & (~(0x1<<5));
			break;
	}
}
delay_ms(int num)
{
	int i,j;
	for(i=num;i>0;i--)
		for(j=1000;j>0;j--);
}

beep_init()
{
	GPD0.CON = (GPD0.CON &(~(0XF<<0))) | (0X2<<0);
	PWM.TCFG0 =  PWM.TCFG0 | 0xff;
	PWM.TCFG1 = (PWM.TCFG1 &(~(0xf<<0))) | (0X2<<0);
	PWM.TCMPB0 = 250;
	PWM.TCNTB0 = 500;
	PWM.TCON = (PWM.TCON &(~(0xf<<0))) | (0x1<<1);
}


beep_on()
{
	PWM.TCON = (PWM.TCON &(~(0xf<<0))) | (0x1<<3 | 0x1<<0);
}
beep_off()
{
	PWM.TCON = (PWM.TCON &(~(0xf<<0))) | (0<<0);
}

#define SYS_SET_FREQUENCE 25000
void beep_set_frequence( unsigned int fre )
{
	//若蜂鸣器的发声频率为0则返回
	if( 0==fre )
		return ;

	PWM.TCMPB0 =  SYS_SET_FREQUENCE/(fre+fre);   //根据设定频率重新设定计数器比较的值
	PWM.TCNTB0 =  SYS_SET_FREQUENCE/fre;			//根据频率重新调整计数值

}




key_init()
{
	GPX1.CON = (GPX1.CON & (~( 0xf << 4)) ) | (0x0<<4);
}


do_irq()
{
	int irqno;
	static int on = 0;
	int data;

	irqno = CPU0.ICCIAR & 0x3ff;

	switch(irqno)
	{
		case 57:

			EXT_INT41_PEND = EXT_INT41_PEND|(0x1<<1); //gpio清中断
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<25);//gic清中断
			break;

		case 42:
			//data = ADCDAT & 0xfff;
			//printf("data = %d\n",data);
			//data *1.8 /4096
			//设备本身清中断
			CLRINTADC = 0x12;
			//第一级中断 combiner
			//IECR2 |= 0x1<<19;
			//GIC
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<10);//gic清中断 42/32
			break;
	}
	CPU0.ICCEOIR = (CPU0.ICCEOIR &(~(0x3ff<<0))) | irqno;//gic清中断
}
key_int_init()
{
	GPX1.CON = (GPX1.CON &(~(0xf<<4))) |(0xf<<4); //引脚设为中断
	EXT_INT41_CON = EXT_INT41_CON &(~(0xf<<4)) |(0x2<<4);//中断触发方式 下降沿
	EXT_INT41_MASK = EXT_INT41_MASK &(~(0x1<<1));   //GPIO使能位

	ICDISER.ICDISER1 |= 0x1<<25; //GIC 针对key这个中断源的 使能
	ICDIPTR.ICDIPTR14 = (ICDIPTR.ICDIPTR14&(~(0xff<<8))) |(0x1<<8);//给key 找一个cpu interface
//57 、4 = 14   1
	ICDDCR = 1;  //分派器的使能开关
	CPU0.ICCPMR = 255; //优先级 masking 高于该优先级才会发送给 cpu
	CPU0.ICCICR = 1;//cpu interface 开关
}

adc_init()
{
	ADCMUX   = 	0x3;
	ADCCON = 1<<16 | 1<<14 | 0xff<<6 ;

	//combiner
	IESR2 |= 0x1<<19;

	ICDISER.ICDISER1 |= 0x1<<10; //   42/32 = 1 10
	ICDIPTR.ICDIPTR10 = (ICDIPTR.ICDIPTR10&(~(0xff<<16))) |(0x1<<16);//给key 找一个cpu interface
//42/4   = 10   2
	ICDDCR = 1;  //分派器的使能开关
	CPU0.ICCPMR = 255; //优先级 masking 高于该优先级才会发送给 cpu
	CPU0.ICCICR = 1;//cpu interface 开关
}


main(int num1,int num2,int num3)
{
	int data ;
	led_init();
	beep_init();
	char buf[32];

	uart_init();

	while(1)
	{
		gets(buf); //输入l1o 表示led第一个灯亮
		
		switch(buf[1])
		{
			case 'l':
				flag = 1;
				break;
			case '2':
				flag = 2;
				break;
			case '3':
				flag = 3;
				break;
			case '4':
				flag = 4;
				break;
		}
	
		switch(buf[0])
		{
			case 'l':
				if(buf[2] == 'o')
				{
					
					led_on(flag);	
				}
				else if(buf[2] == 'f')
				{
					led_off(flag);
				}
				break;


			case 'b': //输入bo 打开蜂鸣器
				if(buf[1] == 'o')
				{	
					beep_on();
				}
				else if(buf[1] == 'f')
				{
					beep_off();
				}

		//	case 'a':

		}
		printf("%s",buf);
	}
#if 0
	adc_init();


	//poll
	while(1)
	{
		ADCCON = ADCCON | 0x1<<0;//开启转换
		delay_ms(200);
		while(!(ADCCON&(0X1<<15)));
		data = ADCDAT & 0xfff;
		//打印

		delay_ms(2000);
	}
#endif
#if 0
	//read  start
	ADCCON = ADCCON | 0x1<<1;  //下一次的转换，由读 ADCDAT寄存器这个动作开启

//	data = ADCDAT;//只是为了开启转换，第一次读的时候 DAT 里的值是随机的，

	while(1)
	{
		delay_ms(2000);
		data = ADCDAT & 0xfff;
		printf("data = %d\n",data);
	}
#endif
#if 0
	//使能 +中断
	ADCCON = ADCCON | 0x1; //开启转换

	while(1)
	{
		delay_ms(2000);
		ADCCON = ADCCON | 0x1;
	}
#endif

#if 0
	int data;

	beep_init();
	key_int_init();

	printf(" stacktop = %x \n&stacktop=%x  \n&stack = %x \n",num1,num2,num3);

	asm(
		"mrs %0,cpsr \n\t"
		:"=r"(data)
		:
	);
	printf("cpsr = %x \n",data);

	while(1);
#endif
#if 0
	key_init();
	beep_init();

	while(1)
	{
		//检查dat寄存器 读取bit 7 的值
		if(!(GPX1.DAT & (0x1<<1)))
		{
			//按下
			delay_ms(10);

			if(!(GPX1.DAT & (0x1<<1))) //消斗
			{
				beep_on();
			}
		}else{
			beep_off();
		}
	}
#endif

#if 0
	unsigned int loop = 0;   //循环控制变量
	unsigned int a = 0;      //计算高中低音使用的变量
	unsigned int b = 0;		 //计算do re mi ....使用的变量
	unsigned int delay = 0;  //计算每一个音延时使用的变量

	beep_init();

	while(1)
	{
		//循环播放 音乐
		for(loop=0; loop<sizeof(happy)/(sizeof(unsigned char)); loop++)
		{
			//查大海的简谱表
			a = happy[loop]>>4 ;       		    //计算出高低音
			b = (happy[loop] & 0x0f )-1;  	    //计算出DO RE MI FA SO LA XI

			delay = 0x1500/time[loop];

			beep_set_frequence( yinyue[a][b] ); //重新设定频率
			beep_on();			//发出一个音
			delay_ms(delay);

			beep_off();         //关闭蜂鸣器， 每个音播放完成后有间隔感
		//	delay_ms(0x10);   //关闭蜂鸣器
			delay_ms(100);
		}

		delay_ms(1000000);  //播放延时
	}
#endif

#if 0
	int i=0;
	int ledno;

	led_init();

	for(i=0;i<1000;i++)
	{
		ledno = i%4 +1 ;

		led_on(ledno);

		delay_ms(1000);
		led_off(ledno);
	}
#endif

#if 0
	//读取cpsr
	asm(
		"mrs %0,cpsr \n\t"
		:"=r"(data)
		:
	);

	printf("cpsr %x \n",data);
#endif
#if 0
	int a =100, b =200, c =0;
	//c =a + b    add  c ,a, b
	asm
	(
		"add %0,%1,%2 \n\t"
		: "=r"(c)
		: "r"(a),"r"(b)
		: "memory"
	);

	printf("c = %d \n",c);



	num = mystrcopy(buf,"hello farsight\n");
	printf("cpsr %x\n",num);

	//使能中断
	asm("mrs r0,cpsr     \n\t"
		"bic r0,r0,#0x80 \n\t"
		"msr cpsr_c  ,r0   \n\t"
	);

	num = mystrcopy(buf,"hello farsight\n");
	printf("cpsr %x\n",num);
#endif
	while(1);
}

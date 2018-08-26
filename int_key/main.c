#include "exynos_4412.h"

//蜂鸣器的初始化
void beep_init()
{
	GPD0.CON = (GPD0.CON &(~(0Xf<<0))) | (0x2<<0); //蜂鸣器对应的gpio config 
	PWM.TCFG0 = PWM.TCFG0 | 0Xff; //pwm 分频
	PWM.TCFG1 = (PWM.TCFG1 &(~(0xf<<0))) | (0X2<<0);
	PWM.TCMPB0 = 250;
	PWM.TCNTB0 = 500;
	PWM.TCON = (PWM.TCON & (~(0Xf<<0))) | (0x1<<1); //指定定时器控制寄存器 ,设置为更新TCMPB0和TCNTB0
}

void key_int_init()
{
	GPX1.CON = (GPX1.CON &(~(0xf<<4))) | (0xf<<4); //配置gpx1 为中断
	EXT_INT41_CON = (EXT_INT41_CON &(~(0xf<<4))) | (0x2<<4); //配置下降沿触发
	EXT_INT41_MASK = EXT_INT41_MASK &(~(0x1<<1));   //GPIO使能位 掩码寄存器

	ICDISER.ICDISER1 |= 0x1<<25; //GIC 针对key这个中断源的 使能 （根据hw_id 选择ppi/spi 中断源）
	ICDIPTR.ICDIPTR14 = (ICDIPTR.ICDIPTR14&(~(0xff<<8))) |(0x1<<8);//根据hw_id找到对应的中断源寄存器并配置cpu interface（查表9-13）

	ICDDCR = 1;  //分派器的使能开关
	CPU0.ICCPMR = 255; //优先级 masking 高于该优先级才会发送给 cpu
	CPU0.ICCICR = 1;//cpu interface 开关
}

//进入中断操作
do_irq()
{
	int irqno;
	static int on = 0;

	irqno = CPU0.ICCIAR & 0x3ff;

	switch(irqno)
	{
		case 57:
			printf("===key====\n");
			if(on == 0)
			{
				beep_on();
				on = 1;
			}else{
				beep_off();
				on = 0;
			}


			EXT_INT41_PEND = EXT_INT41_PEND|(0x1<<1); //gpio清中断
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<25);//gic清中断
			CPU0.ICCEOIR = (CPU0.ICCEOIR &(~(0x3ff<<0))) | irqno;//gic清中断
			break;
	}
}


main()
{
	beep_init();
	key_int_init();

	while(1);
}

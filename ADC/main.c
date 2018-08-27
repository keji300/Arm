#include "exynos_4412.h"

void adc_init()
{
	ADCMUX = 0x3;//3 channel adc
	ADCCON = 1<<16 | 1<<14 | 0xff<<6 ; //分别是精度；分频使能；预分频系数

	//combiner
	//Interrupt Sources中找到 Interrupt Combiner 对应的中断号 INTG10 对应ADC
	//IESR2 使能中断 Interrupt enable set register for group 8 to 11
	IESR2 = IESR2 | (0x1<<19);

	//GIC 中断
	ICDISER.ICDISER1 |= 0x1<<10; //   42/32 = 1 10 //HW_id /32
	ICDIPTR.ICDIPTR10 = (ICDIPTR.ICDIPTR10&(~(0xff<<16))) |(0x1<<16);//给key 找一个cpu interface//42/4   = 10   2
	ICDDCR = 1;  //分派器的使能开关
	CPU0.ICCPMR = 255; //优先级 masking 高于该优先级才会发送给 cpu
	CPU0.ICCICR = 1;//cpu interface 开关

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

delay_ms(int num)
{
	int i,j;
	for(i=num;i>0;i--)
		for(j=1000;j>0;j--);
}

main()
{
	int data;//用来接收ADC的采样数据
	adc_init();
	ADCCON = ADCCON | (0x1<<1); //Enables start by read operation

	while(1)
	{
		delay_ms(1000);
		data = ADCDAT & (0xfff); //读取数据（ADC的数据在一个区间，为了去除其他随机值）
		printf("data = %d\n",data);
	}

}
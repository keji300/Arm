#include "exynos_4412.h"

void adc_init()
{
	ADCMUX = 0x3;//3 channel adc
	ADCCON = 1<<16 | 1<<14 | 0xff<<6 ; //�ֱ��Ǿ��ȣ���Ƶʹ�ܣ�Ԥ��Ƶϵ��

	//combiner
	//Interrupt Sources���ҵ� Interrupt Combiner ��Ӧ���жϺ� INTG10 ��ӦADC
	//IESR2 ʹ���ж� Interrupt enable set register for group 8 to 11
	IESR2 = IESR2 | (0x1<<19);

	//GIC �ж�
	ICDISER.ICDISER1 |= 0x1<<10; //   42/32 = 1 10 //HW_id /32
	ICDIPTR.ICDIPTR10 = (ICDIPTR.ICDIPTR10&(~(0xff<<16))) |(0x1<<16);//��key ��һ��cpu interface//42/4   = 10   2
	ICDDCR = 1;  //��������ʹ�ܿ���
	CPU0.ICCPMR = 255; //���ȼ� masking ���ڸ����ȼ��Żᷢ�͸� cpu
	CPU0.ICCICR = 1;//cpu interface ����

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

			EXT_INT41_PEND = EXT_INT41_PEND|(0x1<<1); //gpio���ж�
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<25);//gic���ж�
			break;

		case 42:
			//data = ADCDAT & 0xfff;
			//printf("data = %d\n",data);
			//data *1.8 /4096
			//�豸�������ж�
			CLRINTADC = 0x12;
			//��һ���ж� combiner
			//IECR2 |= 0x1<<19;
			//GIC
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<10);//gic���ж� 42/32
			break;
	}
	CPU0.ICCEOIR = (CPU0.ICCEOIR &(~(0x3ff<<0))) | irqno;//gic���ж�
}

delay_ms(int num)
{
	int i,j;
	for(i=num;i>0;i--)
		for(j=1000;j>0;j--);
}

main()
{
	int data;//��������ADC�Ĳ�������
	adc_init();
	ADCCON = ADCCON | (0x1<<1); //Enables start by read operation

	while(1)
	{
		delay_ms(1000);
		data = ADCDAT & (0xfff); //��ȡ���ݣ�ADC��������һ�����䣬Ϊ��ȥ���������ֵ��
		printf("data = %d\n",data);
	}

}
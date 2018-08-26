#include "exynos_4412.h"

//�������ĳ�ʼ��
void beep_init()
{
	GPD0.CON = (GPD0.CON &(~(0Xf<<0))) | (0x2<<0); //��������Ӧ��gpio config 
	PWM.TCFG0 = PWM.TCFG0 | 0Xff; //pwm ��Ƶ
	PWM.TCFG1 = (PWM.TCFG1 &(~(0xf<<0))) | (0X2<<0);
	PWM.TCMPB0 = 250;
	PWM.TCNTB0 = 500;
	PWM.TCON = (PWM.TCON & (~(0Xf<<0))) | (0x1<<1); //ָ����ʱ�����ƼĴ��� ,����Ϊ����TCMPB0��TCNTB0
}

void key_int_init()
{
	GPX1.CON = (GPX1.CON &(~(0xf<<4))) | (0xf<<4); //����gpx1 Ϊ�ж�
	EXT_INT41_CON = (EXT_INT41_CON &(~(0xf<<4))) | (0x2<<4); //�����½��ش���
	EXT_INT41_MASK = EXT_INT41_MASK &(~(0x1<<1));   //GPIOʹ��λ ����Ĵ���

	ICDISER.ICDISER1 |= 0x1<<25; //GIC ���key����ж�Դ�� ʹ�� ������hw_id ѡ��ppi/spi �ж�Դ��
	ICDIPTR.ICDIPTR14 = (ICDIPTR.ICDIPTR14&(~(0xff<<8))) |(0x1<<8);//����hw_id�ҵ���Ӧ���ж�Դ�Ĵ���������cpu interface�����9-13��

	ICDDCR = 1;  //��������ʹ�ܿ���
	CPU0.ICCPMR = 255; //���ȼ� masking ���ڸ����ȼ��Żᷢ�͸� cpu
	CPU0.ICCICR = 1;//cpu interface ����
}

//�����жϲ���
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


			EXT_INT41_PEND = EXT_INT41_PEND|(0x1<<1); //gpio���ж�
			ICDICPR.ICDICPR1 = ICDICPR.ICDICPR1 |(0x1<<25);//gic���ж�
			CPU0.ICCEOIR = (CPU0.ICCEOIR &(~(0x3ff<<0))) | irqno;//gic���ж�
			break;
	}
}


main()
{
	beep_init();
	key_int_init();

	while(1);
}

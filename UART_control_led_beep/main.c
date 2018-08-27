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
	//���������ķ���Ƶ��Ϊ0�򷵻�
	if( 0==fre )
		return ;

	PWM.TCMPB0 =  SYS_SET_FREQUENCE/(fre+fre);   //�����趨Ƶ�������趨�������Ƚϵ�ֵ
	PWM.TCNTB0 =  SYS_SET_FREQUENCE/fre;			//����Ƶ�����µ�������ֵ

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
key_int_init()
{
	GPX1.CON = (GPX1.CON &(~(0xf<<4))) |(0xf<<4); //������Ϊ�ж�
	EXT_INT41_CON = EXT_INT41_CON &(~(0xf<<4)) |(0x2<<4);//�жϴ�����ʽ �½���
	EXT_INT41_MASK = EXT_INT41_MASK &(~(0x1<<1));   //GPIOʹ��λ

	ICDISER.ICDISER1 |= 0x1<<25; //GIC ���key����ж�Դ�� ʹ��
	ICDIPTR.ICDIPTR14 = (ICDIPTR.ICDIPTR14&(~(0xff<<8))) |(0x1<<8);//��key ��һ��cpu interface
//57 ��4 = 14   1
	ICDDCR = 1;  //��������ʹ�ܿ���
	CPU0.ICCPMR = 255; //���ȼ� masking ���ڸ����ȼ��Żᷢ�͸� cpu
	CPU0.ICCICR = 1;//cpu interface ����
}

adc_init()
{
	ADCMUX   = 	0x3;
	ADCCON = 1<<16 | 1<<14 | 0xff<<6 ;

	//combiner
	IESR2 |= 0x1<<19;

	ICDISER.ICDISER1 |= 0x1<<10; //   42/32 = 1 10
	ICDIPTR.ICDIPTR10 = (ICDIPTR.ICDIPTR10&(~(0xff<<16))) |(0x1<<16);//��key ��һ��cpu interface
//42/4   = 10   2
	ICDDCR = 1;  //��������ʹ�ܿ���
	CPU0.ICCPMR = 255; //���ȼ� masking ���ڸ����ȼ��Żᷢ�͸� cpu
	CPU0.ICCICR = 1;//cpu interface ����
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
		gets(buf); //����l1o ��ʾled��һ������
		
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


			case 'b': //����bo �򿪷�����
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
		ADCCON = ADCCON | 0x1<<0;//����ת��
		delay_ms(200);
		while(!(ADCCON&(0X1<<15)));
		data = ADCDAT & 0xfff;
		//��ӡ

		delay_ms(2000);
	}
#endif
#if 0
	//read  start
	ADCCON = ADCCON | 0x1<<1;  //��һ�ε�ת�����ɶ� ADCDAT�Ĵ��������������

//	data = ADCDAT;//ֻ��Ϊ�˿���ת������һ�ζ���ʱ�� DAT ���ֵ������ģ�

	while(1)
	{
		delay_ms(2000);
		data = ADCDAT & 0xfff;
		printf("data = %d\n",data);
	}
#endif
#if 0
	//ʹ�� +�ж�
	ADCCON = ADCCON | 0x1; //����ת��

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
		//���dat�Ĵ��� ��ȡbit 7 ��ֵ
		if(!(GPX1.DAT & (0x1<<1)))
		{
			//����
			delay_ms(10);

			if(!(GPX1.DAT & (0x1<<1))) //����
			{
				beep_on();
			}
		}else{
			beep_off();
		}
	}
#endif

#if 0
	unsigned int loop = 0;   //ѭ�����Ʊ���
	unsigned int a = 0;      //������е���ʹ�õı���
	unsigned int b = 0;		 //����do re mi ....ʹ�õı���
	unsigned int delay = 0;  //����ÿһ������ʱʹ�õı���

	beep_init();

	while(1)
	{
		//ѭ������ ����
		for(loop=0; loop<sizeof(happy)/(sizeof(unsigned char)); loop++)
		{
			//��󺣵ļ��ױ�
			a = happy[loop]>>4 ;       		    //������ߵ���
			b = (happy[loop] & 0x0f )-1;  	    //�����DO RE MI FA SO LA XI

			delay = 0x1500/time[loop];

			beep_set_frequence( yinyue[a][b] ); //�����趨Ƶ��
			beep_on();			//����һ����
			delay_ms(delay);

			beep_off();         //�رշ������� ÿ����������ɺ��м����
		//	delay_ms(0x10);   //�رշ�����
			delay_ms(100);
		}

		delay_ms(1000000);  //������ʱ
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
	//��ȡcpsr
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

	//ʹ���ж�
	asm("mrs r0,cpsr     \n\t"
		"bic r0,r0,#0x80 \n\t"
		"msr cpsr_c  ,r0   \n\t"
	);

	num = mystrcopy(buf,"hello farsight\n");
	printf("cpsr %x\n",num);
#endif
	while(1);
}

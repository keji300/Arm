#include "exynos_4412.h"

void uart_init()
{

	/*UART2 initialize*/
	GPA1.CON = (GPA1.CON & ~0xFF ) | (0x22); //GPA1_0:RX;GPA1_1:TX

	UART2.ULCON2 = 0x3; //Normal mode, No parity,One stop bit,8 data bits
	UART2.UCON2 = 0x5;  //Interrupt request or polling mode
	//Baud-rate : src_clock:100Mhz
	UART2.UBRDIV2 = 0x35;  //53    115200
	UART2.UFRACVAL2 = 0x5;
}

void putc(const char data)
{
	while(!(UART2.UTRSTAT2 & 0X2));
	UART2.UTXH2 = data;//缓冲区不为空之后，UTRSTAT2 【1】 就会自动置0
	if (data == '\n')
			putc('\r');
}
void puts(const  char  *pstr)
{
	while(*pstr != '\0')
		putc(*pstr++);
}

//得到串口输入的数据
char getc()
{
	char data;

	while(!(UART2.UTRSTAT2 & 0X1));
	data = UART2.URXH2; //UTRSTAT2  清0

	if((data == '\n') ||(data == '\r'))
	{
		putc('\n');
		putc('\r');

	}else{
		putc(data);
	}
	return data;
}
//             p
//p   1 2 3 \b
void gets(char *p)
{
	char data;

	while((data = getc())!='\r')
	{
		if(data == '\b')//退格
		{
			p--;
			continue;
		}
		*p++ = data;
	}
	if(data == '\r')
	{
		*p++='\n';

	}
	*p = '\0';
}



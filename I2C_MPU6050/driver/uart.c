#include "exynos_4412.h"





void uart_init()
{

	/*UART2 initialize*/
	GPA1.CON = (GPA1.CON & ~0xFF ) | (0x22); //GPA1_0:RX;GPA1_1:TX

	UART2.ULCON2 = 0x3; //Normal mode, No parity,One stop bit,8 data bits

	UART2.UCON2 = 0x5;  //Interrupt request or polling mode
	//Baud-rate : src_clock:100Mhz
	//UART2.UBRDIV2 = 650;
	//UART2.UFRACVAL2 = 1;
	UART2.UBRDIV2 = 0x35;
	UART2.UFRACVAL2 = 0x5;

	//650  1
}

/*
 *
 *Unix系统里，每行结尾只有“<换行>”，即“\n”；
   Windows系统里面，每行结尾是“<回车><换行>”，即“\r\n”；
 *
 */


void putc(const char data)
{

	while(!(UART2.UTRSTAT2 & 0X2));
	UART2.UTXH2 = data;
	if(data == '\n')
	{
		putc('\r');
	}
}
void puts(const  char  *pstr)
{
	while(*pstr != '\0')
		putc(*pstr++);
}

char getc()//12345 \r\n
{
	char data;

	while(!(UART2.UTRSTAT2 & 0X1));
	data = UART2.URXH2;

	if((data == '\n') ||(data == '\r'))
	{
		putc('\n');
		putc('\r');

	}else{
		putc(data);
	}
	return data;
}
void gets(char *p)
{
	char data;

	while((data = getc())!='\r')
	{
		if(data == '\b')
		{
			p--;
		}
		*p++ = data;
	}
	if(data == '\r')
	{
		*p++='\n';
	}
	*p = '\0';
}






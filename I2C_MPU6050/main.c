#include "exynos_4412.h"


//mpu6050 常用的内部寄存器 地址 和 常用的配置值
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取 0x68<<1 | rw

void mydelay_ms(int time)
{
	int i, j;
	while(time--)
	{
		for (i = 0; i < 5; i++)
			for (j = 0; j < 514; j++);
	}
}

//从相应的从机寄存器获取数据
int get_data(unsigned char addr)
{
	char data_h, data_l;
	iic_read(SlaveAddress, addr, &data_h);
	iic_read(SlaveAddress, addr+1, &data_l);
	return (data_h<<8)|data_l;
}

void iic_read(unsigned char slave_addr,unsigned char addr,unsigned char* data)
{
	I2C5.I2CDS = slave_addr;
	I2C5.I2CCON = 1<<7 | 1<<6 | 1<<5;/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */

	//这步就发送了start信号和slave_address+w 
	I2C5.I2CSTAT = 0X3<<6 | 1<<5 | 1<<4;//设置主机发送模式；设置发送状态，设置发送接收使能

	while(!(I2C5.I2CCON & (1<<4) ));

	I2C5.I2CDS = addr;
	I2C5.I2CCON = I2C5.I2CCON & ~(0x1<<4);//这里清中断，并把I2CDS的数据发送出去

	while(!(I2C5.I2CCON & (1<<4) ));
	//这里已经寻址到了需要接收数据的寄存器，开始接收数据，需要重新发送start信号，需要先中止
	I2C5.I2CSTAT = 0xD0;
	I2C5.I2CDS = (slave_addr | (0x1));//最后一位置为1表示开始接收数据
	I2C5.I2CCON = 1<<7 | 1<<6 | 1<<5;/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */

	I2C5.I2CSTAT = 2<<6 | 1<<5 | 1<<4;/*Master receive mode ,START ,ENABLE RX/TX ,*/
	while(!(I2C5.I2CCON & (1<<4)));

	I2C5.I2CCON &= ~((1<<7) | (1<<4));// Resume the operation  & no ack 清中断之前把ack关掉，开始接收数据

	//这里接收外来的数据也是通过中断实现的
	while(!(I2C5.I2CCON & (1<<4)));

	I2C5.I2CSTAT = 0x90;//设置停止位
	I2C5.I2CCON &= ~(1<<4);		//清除中断

	*data = I2C5.I2CDS;
	mydelay_ms(10);





}

//主机发送从机的地址+write命令，从机ack，主机在发送要发送的从机的相应寄存器的地址，从机ack，最后主机发送要传入addr的data
void iic_write(unsigned char slave_addr,unsigned char addr,unsigned char data)
{
	I2C5.I2CDS = slave_addr;//从机的地址
	I2C5.I2CCON = 1<<7 | 1<<6 | 1<<5;/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */

	//这步就发送了start信号和slave_address+w 
	I2C5.I2CSTAT = 0X3<<6 | 1<<5 | 1<<4;//设置主机发送模式；设置发送状态，设置发送接收使能

	//主机发送信号后，从机的物理层实现（verilog）会向主机发送ack信号，这里通过中断实现，中断信号会把中断挂起标志位（Interrupt pending flag）置1
	//也就是I2CCON[4]
	//所以这里等待这个标志位置1，确保收到ack信号
	while(!(I2C5.I2CCON & (1<<4) ));
	//进入到这里表示已经收到ack信号

	//开始新一轮的发送，发送要赋值的寄存器的地址
	I2C5.I2CDS = addr;
	I2C5.I2CCON = I2C5.I2CCON & ~(0x1<<4);//这里清中断，并把I2CDS的数据发送出去

	//同上面的中断操作一样，等待ack
	while(!(I2C5.I2CCON & (1<<4) ));

	I2C5.I2CDS = data;//发送要给寄存器赋值的数据
	I2C5.I2CCON = I2C5.I2CCON & ~(0x1<<4);
	while(!(I2C5.I2CCON & (1<<4) ));


	I2C5.I2CSTAT = 0xD0; //stop  1101 0000 发送停止位

	I2C5.I2CCON &= ~(1<<4);

	mydelay_ms(10);




}

void MPU6050_Init()
{
	iic_write(SlaveAddress, PWR_MGMT_1, 0x00);
	iic_write(SlaveAddress, SMPLRT_DIV, 0x07);
	iic_write(SlaveAddress, CONFIG, 0x06);
	iic_write(SlaveAddress, GYRO_CONFIG, 0x18);
	iic_write(SlaveAddress, ACCEL_CONFIG, 0x01);
}

void main()
{
	int data;//用来接收外设采集到的数据

	//GPIO的两根线作为SCL和SDA
	GPB.CON = (GPB.CON & ~(0xf<<12)) | (0x3<<12); // GPBCON[3] ; [15:12] ; 0x3 = I2C_5_SCL
	GPB.CON = (GPB.CON & ~(0Xf<<8)) | (0x3<<8);  // GPBCON[2]; [11:8]; 0x3 = I2C_5_SDA

	mydelay_ms(100);
	uart_init();

	//I2C状态为 主机发送模式，读/停止状态位的设定（设定总线空闲状态），使能发送接收的开关
	I2C5.I2CSTAT = 0xD0; //Mode selection;[7:6];11 = Master transmit mode   
						//Busy signal status/START STOP condition ;[5] ;0 = (Read) Not busy (If Read) (write) STOP signal generation
						//Serial output ;[4] ; 1 = Enables Rx/Tx
	
	//没有中断挂起（如果读取）2）清除挂起条件并恢复操作（如果写入）
	I2C5.I2CCON &= ~(1<<4);	//Interrupt pending flag; 0 = 1) No interrupt is pending (If Read) 2) Clears pending condition andresumes the operation (If Write)
	

	mydelay_ms(100);
	MPU6050_Init();
	mydelay_ms(100);

	printf("\n********** I2C test!! ***********\n");

	while(1)
	{
	

		data = get_data(GYRO_ZOUT_H);
		printf(" GYRO --> Z <---:Hex: %0x", data);
		printf("\n");


		mydelay_ms(500);
	}

}
#include "exynos_4412.h"


//mpu6050 ���õ��ڲ��Ĵ��� ��ַ �� ���õ�����ֵ
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
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
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I		0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	SlaveAddress	0xD0	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ 0x68<<1 | rw

void mydelay_ms(int time)
{
	int i, j;
	while(time--)
	{
		for (i = 0; i < 5; i++)
			for (j = 0; j < 514; j++);
	}
}

//����Ӧ�Ĵӻ��Ĵ�����ȡ����
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

	//�ⲽ�ͷ�����start�źź�slave_address+w 
	I2C5.I2CSTAT = 0X3<<6 | 1<<5 | 1<<4;//������������ģʽ�����÷���״̬�����÷��ͽ���ʹ��

	while(!(I2C5.I2CCON & (1<<4) ));

	I2C5.I2CDS = addr;
	I2C5.I2CCON = I2C5.I2CCON & ~(0x1<<4);//�������жϣ�����I2CDS�����ݷ��ͳ�ȥ

	while(!(I2C5.I2CCON & (1<<4) ));
	//�����Ѿ�Ѱַ������Ҫ�������ݵļĴ�������ʼ�������ݣ���Ҫ���·���start�źţ���Ҫ����ֹ
	I2C5.I2CSTAT = 0xD0;
	I2C5.I2CDS = (slave_addr | (0x1));//���һλ��Ϊ1��ʾ��ʼ��������
	I2C5.I2CCON = 1<<7 | 1<<6 | 1<<5;/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */

	I2C5.I2CSTAT = 2<<6 | 1<<5 | 1<<4;/*Master receive mode ,START ,ENABLE RX/TX ,*/
	while(!(I2C5.I2CCON & (1<<4)));

	I2C5.I2CCON &= ~((1<<7) | (1<<4));// Resume the operation  & no ack ���ж�֮ǰ��ack�ص�����ʼ��������

	//�����������������Ҳ��ͨ���ж�ʵ�ֵ�
	while(!(I2C5.I2CCON & (1<<4)));

	I2C5.I2CSTAT = 0x90;//����ֹͣλ
	I2C5.I2CCON &= ~(1<<4);		//����ж�

	*data = I2C5.I2CDS;
	mydelay_ms(10);





}

//�������ʹӻ��ĵ�ַ+write����ӻ�ack�������ڷ���Ҫ���͵Ĵӻ�����Ӧ�Ĵ����ĵ�ַ���ӻ�ack�������������Ҫ����addr��data
void iic_write(unsigned char slave_addr,unsigned char addr,unsigned char data)
{
	I2C5.I2CDS = slave_addr;//�ӻ��ĵ�ַ
	I2C5.I2CCON = 1<<7 | 1<<6 | 1<<5;/*ENABLE ACK BIT, PRESCALER:512, ,ENABLE RX/TX */

	//�ⲽ�ͷ�����start�źź�slave_address+w 
	I2C5.I2CSTAT = 0X3<<6 | 1<<5 | 1<<4;//������������ģʽ�����÷���״̬�����÷��ͽ���ʹ��

	//���������źź󣬴ӻ��������ʵ�֣�verilog��������������ack�źţ�����ͨ���ж�ʵ�֣��ж��źŻ���жϹ����־λ��Interrupt pending flag����1
	//Ҳ����I2CCON[4]
	//��������ȴ������־λ��1��ȷ���յ�ack�ź�
	while(!(I2C5.I2CCON & (1<<4) ));
	//���뵽�����ʾ�Ѿ��յ�ack�ź�

	//��ʼ��һ�ֵķ��ͣ�����Ҫ��ֵ�ļĴ����ĵ�ַ
	I2C5.I2CDS = addr;
	I2C5.I2CCON = I2C5.I2CCON & ~(0x1<<4);//�������жϣ�����I2CDS�����ݷ��ͳ�ȥ

	//ͬ������жϲ���һ�����ȴ�ack
	while(!(I2C5.I2CCON & (1<<4) ));

	I2C5.I2CDS = data;//����Ҫ���Ĵ�����ֵ������
	I2C5.I2CCON = I2C5.I2CCON & ~(0x1<<4);
	while(!(I2C5.I2CCON & (1<<4) ));


	I2C5.I2CSTAT = 0xD0; //stop  1101 0000 ����ֹͣλ

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
	int data;//������������ɼ���������

	//GPIO����������ΪSCL��SDA
	GPB.CON = (GPB.CON & ~(0xf<<12)) | (0x3<<12); // GPBCON[3] ; [15:12] ; 0x3 = I2C_5_SCL
	GPB.CON = (GPB.CON & ~(0Xf<<8)) | (0x3<<8);  // GPBCON[2]; [11:8]; 0x3 = I2C_5_SDA

	mydelay_ms(100);
	uart_init();

	//I2C״̬Ϊ ��������ģʽ����/ֹͣ״̬λ���趨���趨���߿���״̬����ʹ�ܷ��ͽ��յĿ���
	I2C5.I2CSTAT = 0xD0; //Mode selection;[7:6];11 = Master transmit mode   
						//Busy signal status/START STOP condition ;[5] ;0 = (Read) Not busy (If Read) (write) STOP signal generation
						//Serial output ;[4] ; 1 = Enables Rx/Tx
	
	//û���жϹ��������ȡ��2����������������ָ����������д�룩
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
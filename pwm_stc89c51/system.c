#include <reg52.h>
#include <system.h>

void delay_ms(uchar t)
{
	uchar j;
	for(j=0;j<250;j++)
	{
		for(t=0;t<100;t++)
		{;}
	}
}

/*��ʼ��API����ʱ����ʼ��API*/

void Timer0Init()		//@11.0592MHz
{
//	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD |= 0x01;		//���ö�ʱ��0ģʽ1
	TL0 = 0x00;			//���ö�ʱ��ֵ
	TH0 = 0x00;			//���ö�ʱ��ֵ
	TF0 = 0;				//���TF0��־
	TR0 = 0;				//��ʱ��0ֹͣ��ʱ
}

void UartInit()		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
//	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
//	AUXR &= 0xFB;		//��ʱ��2ʱ��ΪFosc/12,��12T
	TL2=RCAP2L=(65536-(FOSC/32/BOUD));		//�趨��ʱ��ֵ   �˴�ע�⣬���ݾ���Ƶ���ٸĶ�
	TH2=RCAP2H=(65536-(FOSC/32/BOUD))>>8;		//�趨��ʱ��ֵ
	T2CON=0x34;
//	AUXR |= 0x10;		//������ʱ��2
}

void system_init()
{
	ET0=1;//����ʱ��0�ж� 
	ES=1;	//�������ж�
	EA=1;	//�����ж�
	UartInit();
	Timer0Init();
}

/*���ڷ���API*/
void senddata(uchar dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}
void sendstring(uchar *s)
{
	while(*s)
	{
		senddata(*s++);
	}
}
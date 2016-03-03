#include <stc15f2k60s2.h>
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

void Timer0Init()		//50����@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x00;			//���ö�ʱ��ֵ
	TH0 = 0x4C;			//���ö�ʱ��ֵ
	TF0 = 0;				//���TF0��־
	TR0 = 0;				//��ʱ��0ֹͣ��ʱ
}

void UartInit()		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR &= 0xFB;		//��ʱ��2ʱ��ΪFosc/12,��12T
	T2L = 0xE8;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
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
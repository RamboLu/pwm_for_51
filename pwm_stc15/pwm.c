/*
���ͨ�������Ƶ�ʺ�ռ�ձȿɵ��ڵ�PWM��
1.���ͨ���ɵ�ΪP0.0-P0.7
2.Ƶ����2~2000Hz��Χ���������ڣ�����ֻ��1HZʱռ�ձȴ���65%ʱ��׼ȷ��bug��
3.ռ�ձ���0%~100%֮����������
4.ͨ���ڣ�Ƶ�ʺ�ռ�ձ�ͨ���������������ƣ���ʽΪ  SET 0 2000 090
ʹ��STC15W4K32S4��Ƭ��
*/
#include <stc15f2k60s2.h>
#include <system.h>
#include <stdlib.h>

//����ȫ�ֱ���
uchar Buffer[14]={0};
uchar Buffer_1[14]={'S','E','T',' ','0',' ','2','0','0','0',' ','1','0','0'};//SET 0 2000 100
bit send_flag;		//���ڷ���8λ����
bit flag_error;		//�����ʽ����
uint num,num_high,n;//���ڣ��ߵ�ƽ�����жϴ���
uchar k;
uint updated_fre;					//���յ���Ƶ��ֵ
uint updated_duty_ratio;	//���յ���ռ�ձ�ֵ
uchar updated_channel;

//��������
void pwm_set(uint , uint );	//����PWM�����ú���
uint get_pwm_fre();					//�˺����õ�Ƶ��ֵ
uint get_pwm_duty_ratio();	//�˺����õ�ռ�ձ�ֵ
uchar get_pwm_channel();		//�˺����õ�ͨ��ֵ

//������
void main()
{	
	system_init();
	delay_ms(20);
	sendstring("Welcome!\r\n");				
	sendstring("Please input 'SET x **** XXX';   x is the channel of P0;\r\n");	
	sendstring("**** is the frequency of PWM,2-2000Hz; XXX is duty ratio,0-100% \r\n");
	while(1)
	{			
		if(send_flag)//���յ�10λ����
		{
			send_flag=0;
			flag_error=0;
			if(((Buffer[0]!=Buffer_1[0])||(Buffer[1]!=Buffer_1[1]))||(Buffer[2]!=Buffer_1[2]))
			{
				sendstring("error occured,Please start with SET,like'SET 0 2000 090'\r\n");
				flag_error=1;
			}		
			//����pwm������
			updated_channel=get_pwm_channel();
			updated_fre=get_pwm_fre();
			updated_duty_ratio=get_pwm_duty_ratio();
			//���pwm������Ϣ
			sendstring("pwm_channel is:");
			senddata(updated_channel+'0');
			sendstring("\r\n");
			sendstring("frequency is:");
			senddata(updated_fre/1000+'0');
			senddata(updated_fre/100%10+'0');
			senddata(updated_fre/10%10+'0');
			senddata(updated_fre%10+'0');
			sendstring("Hz \r\n");
			sendstring("duty ratio is:");
			senddata(updated_duty_ratio/100+'0');
			senddata(updated_duty_ratio/10%10+'0');
			senddata(updated_duty_ratio%10+'0');
			sendstring("% \r\n");
			//PWM������ȷ�Լ��
			if(updated_channel>7)
			{
				sendstring("error occured,Please set channel of P0 between 0 and 7,like'SET 0 2000 090'\r\n");
				flag_error=1;
			}
			if(updated_fre<=1||updated_fre>2000)
			{
				sendstring("error occured,Please set frequence between 2 and 2000,like'SET 0 2000 090'\r\n");
				flag_error=1;
			}
			if(updated_duty_ratio>100)
			{
				sendstring("error occured,Please set duty_ratio between 0 and 100,like'SET 0 2000 090'\r\n");
				flag_error=1;
			}
			//�����������޴�������pwm��
			if(flag_error==0)//��ʱ���յ���ȷ��ʽ������
			{
				pwm_set(updated_fre,updated_duty_ratio);//Ƶ��100Hz��ռ�ձ�10
				sendstring("PWM rate has been updated! \r\n \r\n");
			}
		}
	}
}
void pwm_set(uint freq, uint duty_ratio)
{
	unsigned long int pwm_period;
	uint base_time;//ʱ��
	
	TR0=0;
	if(freq>1000)		//1000HZ����Ϊ1000us
	{
		//ʱ���5us
		base_time=5;
		TH0=(65536-5)/256;//��ʱ���Զ�����װ��ֵ
		TL0=(65536-5)%256;
	}
	else if(freq>100)	//100HZ����Ϊ10ms
	{
		//ʱ���10us
		base_time=10;
		TH0=(65536-10)/256;	
		TL0=(65536-10)%256;
	}
	else if(freq>10)	//10HZ����Ϊ100ms
	{
		//ʱ���100us
		base_time=100;
		TH0=(65536-100)/256;
		TL0=(65536-100)%256;
	}
	else				//1HZ����Ϊ1000ms
	{
		//ʱ���1ms
		base_time=1000;
		TH0=(65536-1000)/256;
		TL0=(65536-1000)%256;
	}
	pwm_period=(1000000/freq);					//������ڣ���Χ500-1000000us;(0.5ms-1000ms)
	num=pwm_period/base_time;					//���жϵĴ���������һ������ʱ��
	num_high=num*duty_ratio/100;				//�ߵ�ƽʱ��
	
	num-=8;//������ʱ���ۻ����
	num_high=num_high-(8*duty_ratio/100);//����������ʱ���
	
	TR0=1;//����ʱ��0
}

void timer0() interrupt 1  //��ʱ��0�жϣ�ֻ��һ����ʱ��
{
	n++;
	if(n==num_high)//��ʱ���ڴ�����価����
	{
		//ֻ�е�������num_highʱ���Ÿı�io�ڵ�ƽ
		switch(updated_channel)
		{
			case 0:p0=0;break;
			case 1:p1=0;break;
			case 2:p2=0;break;
			case 3:p3=0;break;
			case 4:p4=0;break;
			case 5:p5=0;break;
			case 6:p6=0;break;
			case 7:p7=0;break;
			default:break;	
		}
	}
	if(n>=num)//һ�����ڽ�������io���ø�
	{
		n=0;
		switch(updated_channel)
		{
			case 0:p0=1;break;
			case 1:p1=1;break;
			case 2:p2=1;break;
			case 3:p3=1;break;
			case 4:p4=1;break;
			case 5:p5=1;break;
			case 6:p6=1;break;
			case 7:p7=1;break;
			default:break;	
		}
	}

}

void uart_isr() interrupt 4
{
	static uchar ii; 
	ES=0;
	if(RI)
	{
		RI=0;
		Buffer[ii]=SBUF;
		ii++;
		if(ii==14)	//	���յ�14���ַ�����λ
		{
			ii=0;
			send_flag=1;
		}
	}
	ES=1;
}

uchar get_pwm_channel()
{
	uchar cha;
	cha=(Buffer[4]-'0');
	return cha;
}
uint get_pwm_fre()
{
	uint fre;
	fre=(Buffer[6]-'0')*1000+(Buffer[7]-'0')*100+(Buffer[8]-'0')*10+(Buffer[9]-'0');
	return fre;
}

uint get_pwm_duty_ratio()
{
	uint wet;
	wet=(Buffer[11]-'0')*100+(Buffer[12]-'0')*10+(Buffer[13]-'0');
	return wet;
}
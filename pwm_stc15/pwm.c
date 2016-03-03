/*
输出通道，输出频率和占空比可调节的PWM波
1.输出通道可调为P0.0-P0.7
2.频率在2~2000Hz范围内连续调节，有且只有1HZ时占空比大于65%时不准确（bug）
3.占空比在0%~100%之间连续调节
4.通道口，频率和占空比通过串口数据来控制，格式为  SET 0 2000 090
使用STC15W4K32S4单片机
*/
#include <stc15f2k60s2.h>
#include <system.h>
#include <stdlib.h>

//定义全局变量
uchar Buffer[14]={0};
uchar Buffer_1[14]={'S','E','T',' ','0',' ','2','0','0','0',' ','1','0','0'};//SET 0 2000 100
bit send_flag;		//串口发送8位数据
bit flag_error;		//输入格式错误
uint num,num_high,n;//周期；高电平；进中断次数
uchar k;
uint updated_fre;					//接收到的频率值
uint updated_duty_ratio;	//接收到的占空比值
uchar updated_channel;

//函数申明
void pwm_set(uint , uint );	//定义PWM波配置函数
uint get_pwm_fre();					//此函数得到频率值
uint get_pwm_duty_ratio();	//此函数得到占空比值
uchar get_pwm_channel();		//此函数得到通道值

//主函数
void main()
{	
	system_init();
	delay_ms(20);
	sendstring("Welcome!\r\n");				
	sendstring("Please input 'SET x **** XXX';   x is the channel of P0;\r\n");	
	sendstring("**** is the frequency of PWM,2-2000Hz; XXX is duty ratio,0-100% \r\n");
	while(1)
	{			
		if(send_flag)//接收到10位数据
		{
			send_flag=0;
			flag_error=0;
			if(((Buffer[0]!=Buffer_1[0])||(Buffer[1]!=Buffer_1[1]))||(Buffer[2]!=Buffer_1[2]))
			{
				sendstring("error occured,Please start with SET,like'SET 0 2000 090'\r\n");
				flag_error=1;
			}		
			//更新pwm波设置
			updated_channel=get_pwm_channel();
			updated_fre=get_pwm_fre();
			updated_duty_ratio=get_pwm_duty_ratio();
			//输出pwm配置信息
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
			//PWM配置正确性检测
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
			//若串口输入无错误，生成pwm波
			if(flag_error==0)//此时接收到正确格式的数据
			{
				pwm_set(updated_fre,updated_duty_ratio);//频率100Hz，占空比10
				sendstring("PWM rate has been updated! \r\n \r\n");
			}
		}
	}
}
void pwm_set(uint freq, uint duty_ratio)
{
	unsigned long int pwm_period;
	uint base_time;//时基
	
	TR0=0;
	if(freq>1000)		//1000HZ周期为1000us
	{
		//时间基5us
		base_time=5;
		TH0=(65536-5)/256;//定时器自动可重装载值
		TL0=(65536-5)%256;
	}
	else if(freq>100)	//100HZ周期为10ms
	{
		//时间基10us
		base_time=10;
		TH0=(65536-10)/256;	
		TL0=(65536-10)%256;
	}
	else if(freq>10)	//10HZ周期为100ms
	{
		//时间基100us
		base_time=100;
		TH0=(65536-100)/256;
		TL0=(65536-100)%256;
	}
	else				//1HZ周期为1000ms
	{
		//时间基1ms
		base_time=1000;
		TH0=(65536-1000)/256;
		TL0=(65536-1000)%256;
	}
	pwm_period=(1000000/freq);					//算得周期，范围500-1000000us;(0.5ms-1000ms)
	num=pwm_period/base_time;					//进中断的次数，代表一个周期时间
	num_high=num*duty_ratio/100;				//高电平时间
	
	num-=8;//补偿定时器累积误差
	num_high=num_high-(8*duty_ratio/100);//尽量修正定时误差
	
	TR0=1;//开定时器0
}

void timer0() interrupt 1  //定时器0中断，只用一个定时器
{
	n++;
	if(n==num_high)//定时器内处理语句尽量少
	{
		//只有当到等于num_high时，才改变io口电平
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
	if(n>=num)//一个周期结束，将io口置高
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
		if(ii==14)	//	接收到14个字符才置位
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
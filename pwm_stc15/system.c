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

/*初始化API，定时器初始化API*/

void Timer0Init()		//50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;			//设置定时初值
	TH0 = 0x4C;			//设置定时初值
	TF0 = 0;				//清除TF0标志
	TR0 = 0;				//定时器0停止计时
}

void UartInit()		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;		//定时器2时钟为Fosc/12,即12T
	T2L = 0xE8;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}

void system_init()
{
	ET0=1;//开定时器0中断 
	ES=1;	//允许串口中断
	EA=1;	//开总中断
	UartInit();
	Timer0Init();
}

/*串口发送API*/
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
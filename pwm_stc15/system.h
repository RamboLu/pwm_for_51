#ifndef _SYSTEM_H
#define _SYSTEM_H

typedef unsigned char uchar;
typedef unsigned int uint;

sbit p0=P0^0;
sbit p1=P0^1;
sbit p2=P0^2;
sbit p3=P0^3;
sbit p4=P0^4;
sbit p5=P0^5;
sbit p6=P0^6;
sbit p7=P0^7;

//¶¨Òåº¯ÊýAPI
void delay_ms(uchar );
void system_init();
void UartInit();	
void Timer0Init();
void senddata(uchar );
void sendstring(uchar *s);

#endif
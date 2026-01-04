/*  文件名称：uart1.c
 * 功    能：CC2530系统实验-----单片机串口发送数据到PC
 * 描    述：实现从 CC2530 上通过串口每3秒发送字串"Hello ，I am CC2530 .\n  "，在PC端实验串口助手来接收数据。使用 CC2530 的串口 UART 0 ，波特率为 57600，其他参数为上电复位默认值。
*/
/* 包含头文件 */
#include "ioCC2530.h"  //定义led灯端口
#define LED1 P1_0     // P1_0定义为P1.0

unsigned int counter=0;       //统计定时器溢出次数
void initUART0(void)
{
  PERCFG = 0x00;	
  P0SEL = 0x3c;	
  U0CSR |= 0x80; 
  U0BAUD = 216;
  U0GCR = 10; 
  U0UCR |= 0x80;  
  UTX0IF = 0;  // 清零UART0 TX中断标志
  EA = 1;   //使能全局中断
}

/************************************************************
 * 函数名称：inittTimer1
 * 功    能：初始化定时器T1控制状态寄存器
*************************************************************/
void inittTimer1()
{  
   CLKCONCMD &= 0x80;   //时钟速度设置为32MHz    
   T1CTL = 0x0E;  // 配置128分频，模比较计数工作模式，并开始启动
   T1CCTL0 |= 0x04;  			//设定timer1通道0比较模式
   T1CC0L =50000 & 0xFF; 	// 把50000的低8位写入T1CC0L
   T1CC0H = ((50000 & 0xFF00) >> 8); // 把50000的高8位写入T1CC0H 
   
   T1IF=0;           //清除timer1中断标志
   T1STAT &= ~0x01;  //清除通道0中断标志
   
   TIMIF &= ~0x40;  //不产生定时器1的溢出中断
   //定时器1的通道0的中断使能T1CCTL0.IM默认使能
   IEN1 |= 0x02;    //使能定时器1的中断  
   EA = 1;        //使能全局中断 
}

void UART0SendByte(unsigned char c)
{
  U0DBUF = c;		
  while (!UTX0IF);  // 等待TX中断标志，即U0DBUF就绪
  UTX0IF = 0;       // 清零TX中断标志 
}

/**************************************************************
 * 函数名称：UART0SendString
 * 功    能：UART0发送一个字符串
**************************************************************/
void UART0SendString(unsigned char *str)
{
  while(*str != '\0')
  {
    UART0SendByte(*str++);   // 发送一字节
  } 
}

/************************************************************
 * 功    能：定时器T1中断服务子程序
 ************************************************************/
#pragma vector = T1_VECTOR //中断服务子程序
__interrupt void T1_ISR(void) 
  { 
    EA = 0;   //禁止全局中断
    counter++;  //统计T1的溢出次数
    T1STAT &= ~0x01;  //清除通道0中断标志
    EA = 1;   //使能全局中断    
}

/************************************************************
 * 函数名称：main
 * 功    能：main函数入口
*******************************************************/
void main(void)
{  
   P1DIR |= 0x01;   /* 配置P1.0的方向为输出 */
   LED1 = 0;
   inittTimer1();  //初始化Timer1
   initUART0();  // UART0初始化  
   while(1) 
     {
		 if(counter>=15)
			{
                          counter=0; 
                          LED1 = 1;
                          UART0SendString("Hello，world\n");
                          LED1 = 0;
                          }		 
     } 
}

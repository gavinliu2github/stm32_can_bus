
#include "stm32f10x.h"
#include "usart1.h"
#include "can.h"
#include "SysTick.h"
#include "LCD9325.h"
#include "stm32f10x_fsmc.h"
#include "delay.h"
__IO uint32_t flag1 = 0xff;		 //用于标志是否接收到数据，在中断函数中赋值
__IO uint32_t flag2 = 0xff;		 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送缓冲区
CanRxMsg RxMessage;				 //接收缓冲区

/*
 *  CAN 差分信号的表示
 *  1:隐性电平   H2.5v - L2.5v = 0v
 *  0:显性电平   H3.5v - L1.5v = 2v
 */

/* 
 * 函数名：main
 * 描述  : "主机"的主函数
 * 输入  ：无
 * 输出  : 无
 */
 
int node1_humi; //结点1湿度
int node1_temp; //结点1温度
int node2_humi;
int node2_temp;

int main(void)
{
	
	/*初始化SysTick定时器*/
	 SysTick_Init();
	
  delay_init();
	
	/*初始化串口模块*/
 	USART1_Config();
  
  //LCD初始化	
	LCD_Init();
  GUI_Show();
	printf( "\r\n 这是 “主机端” 的反馈信息： \r\n");
	
  /* 配置CAN模块 */
 	CAN_Config();
  while(1){
		int count=0;
	for( count=0;count<2;count++)
		{
 		
	/*设置要通过CAN发送的信息*/
	 CAN_SetMsg(0x1314);

//	 printf("\r\n将要发送的报文内容为：\r\n");
//	 printf("\r\n 扩展ID号ExtId：0x%x",TxMessage.ExtId);
//	 printf("\r\n 数据段的内容:Data[0]=0x%x ，Data[1]=0x%x \r\n",TxMessage.Data[0],TxMessage.Data[1]);
	 			
	 /*发送消息 “ABCD”**/
	CAN_Transmit(CAN1, &TxMessage);
	 		
	while( flag1 == 0xff );					//flag1 =0 ,success
	node1_humi=RxMessage.Data[0];
  node1_temp=RxMessage.Data[1];
		LCD_ShowNum(165, 90,node1_humi, 5, 16);
		LCD_ShowNum(165, 70,node1_temp, 5, 16);
//	printf( "\r\n 成功接收到“从机1”返回的数据\r\n ");	
//	printf("\r\n 接收到的报文为：\r\n"); 
//	printf("\r\n 扩展ID号ExtId：0x%x",RxMessage.ExtId);				
	printf("\r\n 数据段的内容:结点1湿度 Data[0]=%d ，结点1温度 Data[1]=%d\r\n",RxMessage.Data[0],RxMessage.Data[1]);
	
//	Delay_ms(1000);


		}
 count=0;
	for(count=0;count<2;count++)
		{
	/*设置要通过CAN发送的信息*/
	 CAN_SetMsg(0x1315);

//	 printf("\r\n将要发送的报文内容为：\r\n");
//	 printf("\r\n 扩展ID号ExtId：0x%x",TxMessage.ExtId);
//	 printf("\r\n 数据段的内容:Data[0]=0x%x ，Data[1]=0x%x \r\n",TxMessage.Data[0],TxMessage.Data[1]);
	 			
	 /*发送消息 “ABCD”**/
	CAN_Transmit(CAN1, &TxMessage);
	//Delay_ms(200); 		
	while( flag2 == 0xff );					//flag2 =0 ,success
  node2_humi=RxMessage.Data[0];
  node2_temp=RxMessage.Data[1];
  LCD_ShowNum(165, 175,node2_humi, 5, 16);
	LCD_ShowNum(165, 155,node2_temp, 5, 16);
//	printf( "\r\n 成功接收到“从机2”返回的数据\r\n ");	
//	printf("\r\n 接收到的报文为：\r\n"); 
//	printf("\r\n 扩展ID号ExtId：0x%x",RxMessage.ExtId);	 
	printf("\r\n 数据段的内容:结点2湿度 Data[0]=%d ，结点2温度 Data[1]=%d\r\n",RxMessage.Data[0],RxMessage.Data[1]);
		
//  Delay_ms(1000);



	}	
	
 }
	
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/



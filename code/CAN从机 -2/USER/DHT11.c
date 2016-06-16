/********************************************************************************
 * 文件名  ：DHT11
 * 描述    ：DHT11 应用函数库         
 * 实验平台：野火STM32开发板
 * 硬件连接：----------------------
 *          |   PD12 - DHT11-DATA   |
 *           ---------------------- 
 * 库版本  ：ST3.5.0
 * 作者    ：保留 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "DHT11.h"

/*
 * 函数名：DHT11_GPIO_Config
 * 描述  ：配置DHT11用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void DHT11_GPIO_Config(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOD的外设时钟*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE); 

	/*选择要控制的GPIOD引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化GPIOD*/
  	GPIO_Init(GPIOD, &GPIO_InitStructure);		  

	/* 拉高GPIOD12	*/
	GPIO_SetBits(GPIOD, GPIO_Pin_12);	 
}

/*
 * 函数名：DHT11_Mode_IPU
 * 描述  ：使DHT11-DATA引脚变为输入模式
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;

	  	/*选择要控制的GPIOD引脚*/	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;

	   /*设置引脚模式为浮空输入模式*/ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 

	  /*调用库函数，初始化GPIOD*/
	  GPIO_Init(GPIOD, &GPIO_InitStructure);	 
}

/*
 * 函数名：DHT11_Mode_Out_PP
 * 描述  ：使DHT11-DATA引脚变为输出模式
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	 	/*选择要控制的GPIOD引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*调用库函数，初始化GPIOD*/
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	 	 
}


static uint8_t Read_Byte(void)
{	  

   	 uint8_t i, temp=0;

	 for(i=0;i<8;i++)    
	 {
	 
	   /*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
    	while(DHT11_DATA_IN()==Bit_RESET);
		 
		 /*DHT11 以27~28us的高电平表示“0”，以70us高电平表示“1”，
	 		 通过检测60us后的电平即可区别这两个状态*/

		Delay_us(6); //延时60us		   	  
	
		  if(DHT11_DATA_IN()==Bit_SET)//60us后仍为高电平表示数据“1”
		   {
		   	/*轮询直到从机发出的剩余的 30us 高电平结束*/
   				 while(DHT11_DATA_IN()==Bit_SET);

				 temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1 
			
		   }
		   else	 //60us后为低电平表示数据“0”
		   {			   
		   	  temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0
		   }
	 }
	  return temp;
}

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	/*输出模式*/
   DHT11_Mode_Out_PP();
   /*主机拉低*/
   DHT11_DATA_OUT(LOW);
   /*延时18ms*/
   Delay_ms(18);

   /*总线拉高 主机延时30us*/
   DHT11_DATA_OUT(HIGH); 

   Delay_us(3);   //延时30us
   
 	/*主机设为输入 判断从机响应信号*/ 
   DHT11_Mode_IPU();

 /*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
   if(DHT11_DATA_IN()==Bit_RESET)   //T !   
    {
  
	
	  /*轮询直到从机发出 的80us 低电平 响应信号结束*/  
	    while(DHT11_DATA_IN()==Bit_RESET);
	  	 
	  /*轮询直到从机发出的 80us 高电平 标置信号结束*/
	    while(DHT11_DATA_IN()==Bit_SET);

	  /*开始接收数据*/   
	 	 DHT11_Data->humi_int= Read_Byte();
								
		 DHT11_Data->humi_deci= Read_Byte();
				
		 DHT11_Data->temp_int= Read_Byte();

		 DHT11_Data->temp_deci= Read_Byte();
				 		
		 DHT11_Data->check_sum= Read_Byte();
		 								  

		 	/*读取结束，引脚改为输出模式*/
  		 DHT11_Mode_Out_PP();
		   /*主机拉高*/
		 DHT11_DATA_OUT(HIGH);

		   /*检查读取的数据是否正确*/
		 if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
		  	return SUCCESS;
		  else 
		  	return ERROR;
   }
   else
   	{		
			return ERROR;
		}
   
}

	  


/*************************************END OF FILE******************************/


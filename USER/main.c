/*
STM32F103RCT6
iic通信0.96寸OLED显示屏；显示中英文、数字、图片
HC-SR04超声波传感器

OLED接线定义:
					VCC--3.3V/5V
					GND--GND
					SCL--PB8
					SDA--PB9
HC-SR04超声波接线定义:
					Trig--PB7
					Echo--PB6
土壤湿度传感器接线定义:
                    VOUT--PA1
红外线传感器接线定义:
                    hongwai--PA6
LED指示灯定义:
                    baojing1--PA5
					baojing2--PB5
蜂鸣器定义:
                    BEEP--PA7
*/
#include "stm32f10x.h"  //包含需要的头文件
#include "sys.h"  
#include "delay.h"
#include "oled_iic.h"
#include "stdio.h"
#include "wave.h"
#include "timer.h"
#include "usart1.h"
#include "usart2.h"
#include "mqtt.h"       //包含需要的头文件
#include "timer1.h"     //包含需要的头文件
#include "timer2.h"     //包含需要的头文件
#include "timer3.h"     //包含需要的头文件
#include "timer4.h"     //包含需要的头文件
#include "wifi.h"	    	//包含需要的头文件
#include "adc.h"
#include "led.h"
#include "beep.h"
u8 xianshi_status=3;
u8 turang_shidu=13;
u8 CurrentTemperature=38;  	//温度    
u8 CurrentHumidity=28; 			//湿度
int main(void)
{
    u16 adcx;
    float shidu=0.0f;	
	u8 t=20;//trig引脚高电平持续时间   	 		 		
	u8 Distance;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();                   //延时功能初始化
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	Usart2_Init(115200);            //串口2功能初始化，波特率115200	
	TIM4_Init(300,7200);            //TIM4初始化，定时时间 300*7200*1000/72000000 = 30ms
	Wave_SRD_Init();//初始化超声波
	//Timer_SRD_Init(9998,7199);//定时器初始化	
    Adc_Init();		  		//ADC初始化
	LED_Init();			     //LED端口初始
	BEEP_Init();             //蜂鸣器初始化
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
    MQTT_Buff_Init();               //初始化接收,发送,命令数据的 缓冲区 以及各状态参数
	AliIoT_Parameter_Init();	     //初始化连接OneNet云IoT平台MQTT服务器的参数
	extern const u8 BMP1[];

	OLED_Init();		 	//OLED屏幕初始化			
	
//	while(Wave_SRD_Strat(t)) //等待初始化
//	{
//	}	
	OLED_Clear();	
	OLED_ShowCH(0,0,"土壤湿度:");
	OLED_ShowCH(88,0,"%");
	OLED_ShowCH(0,3,"动物方位:");			
	OLED_ShowCH(0,6,"距离:");		
    OLED_ShowCH(100,6,"cm");		
	while(1)
	{	
				/*--------------------------------------------------------------------*/
		/*   Connect_flag=1同服务器建立了连接,我们可以发布数据和接收推送了    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if成立的话，说明发送缓冲区有数据了
				//3种情况可进入if
				//第1种：0x10 连接报文
				//第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
				//第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
				if((MQTT_TxDataOutPtr[1]==0x10)||((MQTT_TxDataOutPtr[1]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[1]);  //串口提示信息
					MQTT_TxData(MQTT_TxDataOutPtr);                       //发送数据
					MQTT_TxDataOutPtr += BUFF_UNIT;                       //指针下移
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //如果指针到缓冲区尾部了
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //指针归位到缓冲区开头
				} 				
			}//处理发送缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if成立的话，说明接收缓冲区有数据了														
				u1_printf("接收到数据:");
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/	
				//MQTT_RxDataOutPtr[0]为从MQTT_RxDataOutPtr[1]开始直到最后的数据的字节数
				//if判断，如果一共接收了4个字节，第一个字节是0x20，表示收到的是CONNACK报文
				//接着我们要判断第4个字节，看看CONNECT报文是否成功
				if((MQTT_RxDataOutPtr[0]==4)&&(MQTT_RxDataOutPtr[1]==0x20)){             			
				    switch(MQTT_RxDataOutPtr[4]){					
						case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
								    ConnectPack_flag = 1;                                        //CONNECT报文成功，订阅报文可发
									break;                                                       //跳出分支case 0x00                                              
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x01   
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x02 
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x03
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x04
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x05 		
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
									Connect_flag = 0;                                            //Connect_flag置零，重启连接					
									break;                                                       //跳出分支case default 								
					}				
				}			
				//if判断，如果一共接收了5个字节，第一个字节是0x90，表示收到的是SUBACK报文
				//接着我们要判断订阅回复，看看是不是成功
				else if((MQTT_RxDataOutPtr[0]==5)&&(MQTT_RxDataOutPtr[1]==0x90)){ 
						switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 :
						case 0x01 : u1_printf("订阅成功\r\n");            //串口输出信息
												SubcribePack_flag = 1;                //SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
												Ping_flag = 0;                        //Ping_flag清零
												TIM3_ENABLE_30S();                    //启动30s的PING定时器
									break;                                //跳出分支                                             
						default   : u1_printf("订阅失败，准备重启\r\n");  //串口输出信息 
												Connect_flag = 0;                     //Connect_flag置零，重启连接
									break;                                //跳出分支 								
					}					
				}
				//if判断，如果一共接收了2个字节，第一个字节是0xD0，表示收到的是PINGRESP报文
				else if((MQTT_RxDataOutPtr[0]==2)&&(MQTT_RxDataOutPtr[1]==0xD0)){ 
					u1_printf("PING报文回复\r\n"); 		  //串口输出信息 
					if(Ping_flag==1){                     //如果Ping_flag=1，表示第一次发送
						 Ping_flag = 0;    				  //要清除Ping_flag标志
					}else if(Ping_flag>1){ 				  //如果Ping_flag>1，表示是多次发送了，而且是2s间隔的快速发送
						Ping_flag = 0;     				  //要清除Ping_flag标志
						TIM3_ENABLE_30S(); 				  //PING定时器重回30s的时间
					}				
				}	
				//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
				//我们要提取控制命令
				else if((MQTT_RxDataOutPtr[1]==0x30)){ 
					u1_printf("服务器等级0推送\r\n"); 		   //串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				}				
								
				MQTT_RxDataOutPtr += BUFF_UNIT;                     //指针下移
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //如果指针到缓冲区尾部了
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //指针归位到缓冲区开头                        
			}//处理接收缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理命令缓冲区数据                      */
			/*-------------------------------------------------------------*/
//			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if成立的话，说明命令缓冲区有数据了			
//					u1_printf("命令:%s\r\n",&MQTT_CMDOutPtr[1]);                 //串口输出信息
//			if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch1\":1}")){	           //如果搜索到"params":{"PowerSwitch":1}说明服务器下发打开开关1	
//					LED1_ON;                                                                   //打开LED1
//					LED1_State();                                                              //判断开关状态，并发布给服务器  
//				}else if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch1\":0}")){       //如果搜索到"params":{"PowerSwitch":0}说明服务器下发关闭开关1
//					LED1_OFF;                                                                  //关闭LED1
//					LED1_State();                                                              //判断开关状态，并发布给服务器  
//				}else if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch2\":1}")){ 	   //如果搜索到"params":{"PowerSwitch_2":1}说明服务器下发打开开关2	
//					LED2_ON;                                                                   //打开LED2
//					LED2_State();                                                              //判断开关状态，并发布给服务器  
//				}else if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch2\":0}")){       //如果搜索到"params":{"PowerSwitch_2":0}说明服务器下发关闭开关2
//					LED2_OFF;                                                                  //关闭LED2
//					LED2_State();                                                              //判断开关状态，并发布给服务器  
//				}	
//				
//				MQTT_CMDOutPtr += BUFF_UNIT;                             	 //指针下移
//				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //如果指针到缓冲区尾部了
//					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //指针归位到缓冲区开头				
//			}//处理命令缓冲区数据的else if分支结尾	
		}//Connect_flag=1的if分支的结尾	
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("需要连接服务器\r\n");                 //串口输出信息
			TIM_Cmd(TIM4,DISABLE);                           //关闭TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //关闭TIM3  
			WiFi_RxCounter=0;                                //WiFi接收数据量变量清零                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //清空WiFi接收缓冲区 
			if(WiFi_Connect_IoTServer()==0){   			     //如果WiFi连接云服务器函数返回0，表示正确，进入if
				u1_printf("建立TCP连接成功\r\n");            //串口输出信息
				Connect_flag = 1;                            //Connect_flag置1，表示连接成功	
				WiFi_RxCounter=0;                            //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //清空WiFi接收缓冲区 
				MQTT_Buff_ReInit();                          //重新初始化发送缓冲区 
				TIM2_ENABLE_1S();
				//LED_State();				
			}				
		}
		
		/*--------------------------------------------------------------------*/
		/*                            其他控制程序                            */
		/*--------------------------------------------------------------------*/
		//湿度显示
//        adcx=Get_Adc_Average(ADC_Channel_1,10);//得到采集以后的电压数据
//        shidu=(float)adcx*(1.0/4096);
//        shidu*=100;//得到湿度百分比
//		turang_shidu=(int)shidu;
//        OLED_ShowNum(72,0,(int)shidu,6,16);	//显示湿度数据		
//        //显示模式判断
//		switch(xianshi_status)
//		{
//			case 1:OLED_ShowCH(72,3,"超声波"),LED0=0,BEEP=0;break;
//            case 2:OLED_ShowCH(72,3,"红外"),LED1=0,BEEP=0;break;
//            case 3:OLED_ShowCH(72,3,"超声波&红外"),LED0=0,LED1=0,BEEP=0;break;
//            default:OLED_ShowCH(72,3,"无"),LED0=1,LED1=1,BEEP=1;break;			
//		}
//		//产生高于10Us的高电平与OLED上显示超声波所测距离		
//		Wave_SRD_Strat(t);
//		Distance = TIM_GetCounter(TIM3)*340/200.0;
//		if(Distance>0)
//		{
//			OLED_ShowNum(50,6,Distance,6,16);		//显示超声波测量距离
//			delay_ms(10);
//		}
//		//判断超声波是否检测到物体
//		if(Distance<=20)
//		{
//			if(hongwai==1)
//			xianshi_status=3;
//			else
//		    xianshi_status=1;
//		}
//		//判断红外传感是否检测到物体
//		if(hongwai==1)
//		{
//			if(Distance<=20)
//			xianshi_status=3;
//			else
//			xianshi_status=2;
//		}
	}
}



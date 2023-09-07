/*
STM32F103RCT6
iicͨ��0.96��OLED��ʾ������ʾ��Ӣ�ġ����֡�ͼƬ
HC-SR04������������

OLED���߶���:
					VCC--3.3V/5V
					GND--GND
					SCL--PB8
					SDA--PB9
HC-SR04���������߶���:
					Trig--PB7
					Echo--PB6
����ʪ�ȴ��������߶���:
                    VOUT--PA1
�����ߴ��������߶���:
                    hongwai--PA6
LEDָʾ�ƶ���:
                    baojing1--PA5
					baojing2--PB5
����������:
                    BEEP--PA7
*/
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "sys.h"  
#include "delay.h"
#include "oled_iic.h"
#include "stdio.h"
#include "wave.h"
#include "timer.h"
#include "usart1.h"
#include "usart2.h"
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "timer1.h"     //������Ҫ��ͷ�ļ�
#include "timer2.h"     //������Ҫ��ͷ�ļ�
#include "timer3.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�
#include "wifi.h"	    	//������Ҫ��ͷ�ļ�
#include "adc.h"
#include "led.h"
#include "beep.h"
u8 xianshi_status=3;
u8 turang_shidu=13;
u8 CurrentTemperature=38;  	//�¶�    
u8 CurrentHumidity=28; 			//ʪ��
int main(void)
{
    u16 adcx;
    float shidu=0.0f;	
	u8 t=20;//trig���Ÿߵ�ƽ����ʱ��   	 		 		
	u8 Distance;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();                   //��ʱ���ܳ�ʼ��
	Usart1_Init(9600);              //����1���ܳ�ʼ����������9600
	Usart2_Init(115200);            //����2���ܳ�ʼ����������115200	
	TIM4_Init(300,7200);            //TIM4��ʼ������ʱʱ�� 300*7200*1000/72000000 = 30ms
	Wave_SRD_Init();//��ʼ��������
	//Timer_SRD_Init(9998,7199);//��ʱ����ʼ��	
    Adc_Init();		  		//ADC��ʼ��
	LED_Init();			     //LED�˿ڳ�ʼ
	BEEP_Init();             //��������ʼ��
	WiFi_ResetIO_Init();            //��ʼ��WiFi�ĸ�λIO
    MQTT_Buff_Init();               //��ʼ������,����,�������ݵ� ������ �Լ���״̬����
	AliIoT_Parameter_Init();	     //��ʼ������OneNet��IoTƽ̨MQTT�������Ĳ���
	extern const u8 BMP1[];

	OLED_Init();		 	//OLED��Ļ��ʼ��			
	
//	while(Wave_SRD_Strat(t)) //�ȴ���ʼ��
//	{
//	}	
	OLED_Clear();	
	OLED_ShowCH(0,0,"����ʪ��:");
	OLED_ShowCH(88,0,"%");
	OLED_ShowCH(0,3,"���﷽λ:");			
	OLED_ShowCH(0,6,"����:");		
    OLED_ShowCH(100,6,"cm");		
	while(1)
	{	
				/*--------------------------------------------------------------------*/
		/*   Connect_flag=1ͬ����������������,���ǿ��Է������ݺͽ���������    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     �����ͻ���������                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if�����Ļ���˵�����ͻ�������������
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ���
				//��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
				//��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
				if((MQTT_TxDataOutPtr[1]==0x10)||((MQTT_TxDataOutPtr[1]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[1]);  //������ʾ��Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);                       //��������
					MQTT_TxDataOutPtr += BUFF_UNIT;                       //ָ������
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //���ָ�뵽������β����
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //ָ���λ����������ͷ
				} 				
			}//�����ͻ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if�����Ļ���˵�����ջ�������������														
				u1_printf("���յ�����:");
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/	
				//MQTT_RxDataOutPtr[0]Ϊ��MQTT_RxDataOutPtr[1]��ʼֱ���������ݵ��ֽ���
				//if�жϣ����һ��������4���ֽڣ���һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if((MQTT_RxDataOutPtr[0]==4)&&(MQTT_RxDataOutPtr[1]==0x20)){             			
				    switch(MQTT_RxDataOutPtr[4]){					
						case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ������ı��Ŀɷ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}				
				}			
				//if�жϣ����һ��������5���ֽڣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if((MQTT_RxDataOutPtr[0]==5)&&(MQTT_RxDataOutPtr[1]==0x90)){ 
						switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 :
						case 0x01 : u1_printf("���ĳɹ�\r\n");            //���������Ϣ
												SubcribePack_flag = 1;                //SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
												Ping_flag = 0;                        //Ping_flag����
												TIM3_ENABLE_30S();                    //����30s��PING��ʱ��
									break;                                //������֧                                             
						default   : u1_printf("����ʧ�ܣ�׼������\r\n");  //���������Ϣ 
												Connect_flag = 0;                     //Connect_flag���㣬��������
									break;                                //������֧ 								
					}					
				}
				//if�жϣ����һ��������2���ֽڣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if((MQTT_RxDataOutPtr[0]==2)&&(MQTT_RxDataOutPtr[1]==0xD0)){ 
					u1_printf("PING���Ļظ�\r\n"); 		  //���������Ϣ 
					if(Ping_flag==1){                     //���Ping_flag=1����ʾ��һ�η���
						 Ping_flag = 0;    				  //Ҫ���Ping_flag��־
					}else if(Ping_flag>1){ 				  //���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						Ping_flag = 0;     				  //Ҫ���Ping_flag��־
						TIM3_ENABLE_30S(); 				  //PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if((MQTT_RxDataOutPtr[1]==0x30)){ 
					u1_printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
								
				MQTT_RxDataOutPtr += BUFF_UNIT;                     //ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //���ָ�뵽������β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //ָ���λ����������ͷ                        
			}//������ջ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ���������������                      */
			/*-------------------------------------------------------------*/
//			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if�����Ļ���˵�����������������			
//					u1_printf("����:%s\r\n",&MQTT_CMDOutPtr[1]);                 //���������Ϣ
//			if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch1\":1}")){	           //���������"params":{"PowerSwitch":1}˵���������·��򿪿���1	
//					LED1_ON;                                                                   //��LED1
//					LED1_State();                                                              //�жϿ���״̬����������������  
//				}else if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch1\":0}")){       //���������"params":{"PowerSwitch":0}˵���������·��رտ���1
//					LED1_OFF;                                                                  //�ر�LED1
//					LED1_State();                                                              //�жϿ���״̬����������������  
//				}else if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch2\":1}")){ 	   //���������"params":{"PowerSwitch_2":1}˵���������·��򿪿���2	
//					LED2_ON;                                                                   //��LED2
//					LED2_State();                                                              //�жϿ���״̬����������������  
//				}else if(strstr((char *)MQTT_CMDOutPtr+1,"\"params\":{\"Switch2\":0}")){       //���������"params":{"PowerSwitch_2":0}˵���������·��رտ���2
//					LED2_OFF;                                                                  //�ر�LED2
//					LED2_State();                                                              //�жϿ���״̬����������������  
//				}	
//				
//				MQTT_CMDOutPtr += BUFF_UNIT;                             	 //ָ������
//				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //���ָ�뵽������β����
//					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //ָ���λ����������ͷ				
//			}//��������������ݵ�else if��֧��β	
		}//Connect_flag=1��if��֧�Ľ�β	
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0ͬ�������Ͽ�������,����Ҫ�������ӷ�����         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("��Ҫ���ӷ�����\r\n");                 //���������Ϣ
			TIM_Cmd(TIM4,DISABLE);                           //�ر�TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //�ر�TIM3  
			WiFi_RxCounter=0;                                //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
			if(WiFi_Connect_IoTServer()==0){   			     //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
				u1_printf("����TCP���ӳɹ�\r\n");            //���������Ϣ
				Connect_flag = 1;                            //Connect_flag��1����ʾ���ӳɹ�	
				WiFi_RxCounter=0;                            //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
				MQTT_Buff_ReInit();                          //���³�ʼ�����ͻ����� 
				TIM2_ENABLE_1S();
				//LED_State();				
			}				
		}
		
		/*--------------------------------------------------------------------*/
		/*                            �������Ƴ���                            */
		/*--------------------------------------------------------------------*/
		//ʪ����ʾ
//        adcx=Get_Adc_Average(ADC_Channel_1,10);//�õ��ɼ��Ժ�ĵ�ѹ����
//        shidu=(float)adcx*(1.0/4096);
//        shidu*=100;//�õ�ʪ�Ȱٷֱ�
//		turang_shidu=(int)shidu;
//        OLED_ShowNum(72,0,(int)shidu,6,16);	//��ʾʪ������		
//        //��ʾģʽ�ж�
//		switch(xianshi_status)
//		{
//			case 1:OLED_ShowCH(72,3,"������"),LED0=0,BEEP=0;break;
//            case 2:OLED_ShowCH(72,3,"����"),LED1=0,BEEP=0;break;
//            case 3:OLED_ShowCH(72,3,"������&����"),LED0=0,LED1=0,BEEP=0;break;
//            default:OLED_ShowCH(72,3,"��"),LED0=1,LED1=1,BEEP=1;break;			
//		}
//		//��������10Us�ĸߵ�ƽ��OLED����ʾ�������������		
//		Wave_SRD_Strat(t);
//		Distance = TIM_GetCounter(TIM3)*340/200.0;
//		if(Distance>0)
//		{
//			OLED_ShowNum(50,6,Distance,6,16);		//��ʾ��������������
//			delay_ms(10);
//		}
//		//�жϳ������Ƿ��⵽����
//		if(Distance<=20)
//		{
//			if(hongwai==1)
//			xianshi_status=3;
//			else
//		    xianshi_status=1;
//		}
//		//�жϺ��⴫���Ƿ��⵽����
//		if(hongwai==1)
//		{
//			if(Distance<=20)
//			xianshi_status=3;
//			else
//			xianshi_status=2;
//		}
	}
}



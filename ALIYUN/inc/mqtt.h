/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H



#define  BUFF_UNIT           512    //����������
#define  R_NUM               8     //���ջ���������
#define  T_NUM               8     //���ͻ���������  
#define  C_NUM               8     //�����������

#define  MQTT_TxData(x)       u2_TxData(x)                                  //����2����800C�����ݷ���
//WEB��
//#define  PRODUCTKEY           "a1HdDTHXVco"                                 //��ƷID
//#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                            //��ƷID����
//#define  DEVICENAME           "D002"                                  			//�豸��  
//#define  DEVICENAME_LEN       strlen(DEVICENAME)                            //�豸������
//#define  DEVICESECRE          "HfmVeXoIGJbdubU4A19T7ehaLylI49uJ"            //�豸��Կ   
//#define  DEVICESECRE_LEN      strlen(DEVICESECRE)  
//#define  S_TOPIC_NAME         "/sys/a1HdDTHXVco/D002/thing/service/property/set"   						//��Ҫ���ĵ�����  
//#define  P_TOPIC_NAME         "/sys/a1HdDTHXVco/D002/thing/event/property/post"        				//��Ҫ����������    

//APP
//#define  PRODUCTKEY           "a14TB0cJFjt"                                 //��ƷID
//#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                            //��ƷID����
//#define  DEVICENAME           "D001"                                  			//�豸��  
//#define  DEVICENAME_LEN       strlen(DEVICENAME)                            //�豸������
//#define  DEVICESECRE          "U2pHzMTnov7cQvDRvHvqrhaQjaTjNnUv"            //�豸��Կ   
//#define  DEVICESECRE_LEN      strlen(DEVICESECRE)  
//                         
//#define  S_TOPIC_NAME         "/sys/a14TB0cJFjt/D001/thing/service/property/set"   						//��Ҫ���ĵ�����  
//#define  P_TOPIC_NAME         "/sys/a14TB0cJFjt/D001/thing/event/property/post"   
 



#define  PRODUCTKEY           "a1f77yZF9oA"                                 //��ƷID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                            //��ƷID����
#define  DEVICENAME           "example_1"                                  			//�豸��  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                            //�豸������
#define  DEVICESECRE          "19fee7f8703dfa93acf217580fb442d5"            //�豸��Կ   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)  
                         
#define  S_TOPIC_NAME         "/sys/a1f77yZF9oA/example_1/thing/service/property/set"   						//��Ҫ���ĵ�����  
#define  P_TOPIC_NAME         "/sys/a1f77yZF9oA/example_1/thing/event/property/post"   

extern unsigned char  MQTT_RxDataBuf[R_NUM][BUFF_UNIT];        //�ⲿ�������������ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_RxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_RxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_RxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_TxDataBuf[T_NUM][BUFF_UNIT];        //�ⲿ�������������ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_TxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_TxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_TxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_CMDBuf[C_NUM][BUFF_UNIT];           //�ⲿ�����������������ݵĽ��ջ�����
extern unsigned char *MQTT_CMDInPtr;                           //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_CMDOutPtr;                          //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_CMDEndPtr;                          //�ⲿ����������ָ�򻺳���������λ��

extern char ClientID[128];     //�ⲿ������������ſͻ���ID�Ļ�����
extern int  ClientID_len;      //�ⲿ������������ſͻ���ID�ĳ���
extern char Username[128];     //�ⲿ��������������û����Ļ�����
extern int  Username_len;	   //�ⲿ��������������û����ĳ���
extern char Passward[128];     //�ⲿ�����������������Ļ�����
extern int  Passward_len;	   //�ⲿ�����������������ĳ���
extern char ServerIP[128];     //�ⲿ������������ŷ�����IP��������
extern int  ServerPort;        //�ⲿ������������ŷ������Ķ˿ں�

extern char Ping_flag;         //�ⲿ����������ping����״̬      0������״̬���ȴ���ʱʱ�䵽������Ping����
                               //�ⲿ����������ping����״̬      1��Ping�����ѷ��ͣ����յ� �������ظ����ĵĺ� ��1��Ϊ0
extern char Connect_flag;      //�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
extern char ReConnect_flag;    //�ⲿ��������������������״̬    0�����ӻ�����  1�����ӶϿ�������
extern char ConnectPack_flag;  //�ⲿ����������CONNECT����״̬   1��CONNECT���ĳɹ�
extern char SubcribePack_flag; //�ⲿ�������������ı���״̬      1�����ı��ĳɹ�

void MQTT_Buff_Init(void);
void MQTT_Buff_ReInit(void);
void AliIoT_Parameter_Init(void);
void MQTT_ConectPack(void);
void MQTT_Subscribe(char *, int);
void MQTT_PingREQ(void);
void MQTT_PublishQs0(char *, char *, int);
void MQTT_DealPushdata_Qs0(unsigned char *);	
void TxDataBuf_Deal(unsigned char *, int);
void CMDBuf_Deal(unsigned char *, int);

#endif

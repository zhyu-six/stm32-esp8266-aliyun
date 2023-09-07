#include "timer.h"
#include "sys.h"
//��ʼ����ʱ��
void Timer_SRD_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	TIM_BaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_BaseInitStruct.TIM_Period=arr;
	TIM_BaseInitStruct.TIM_Prescaler=psc;
	TIM_BaseInitStruct.TIM_ClockDivision=0;
	TIM_TimeBaseInit(TIM5,&TIM_BaseInitStruct);
	//�ж����ȼ�����
	NVIC_InitStruct.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	//���������жϣ������ж�
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	TIM_ITConfig(TIM5,TIM_IT_Trigger,ENABLE);
	
	TIM_Cmd(TIM5,ENABLE);//ʹ���ж�
	
}
//�жϷ�����
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
	}
	
}























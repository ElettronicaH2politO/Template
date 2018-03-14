#include "can2_config.h"

typedef union data{
	float f;
	uint8_t s[4];
}myData;

CanTxMsg TxMessage;
CanRxMsg RxMessage;

void initCAN2(void){
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  CAN_DeInit(CAN2);

  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_15tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
  CAN_InitStructure.CAN_Prescaler = 10;
  CAN_Init(CAN2, &CAN_InitStructure);

  CAN_FilterInitStructure.CAN_FilterNumber = 14;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void CANBus_Send(uint32_t id, float bqAux1, float bqAux2){
	/*uint8_t* arr;
	arr = (uint8_t*)(&f);*/
	
	myData arr1;
	myData arr2;
	arr1.f = bqAux1;
	arr2.f = bqAux2;
	
	TxMessage.StdId = id;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
	
	for(int i = 0; i < 8; i++){
		if( i <= 3 ){
			TxMessage.Data[i] = arr1.s[i];
		}else{
			TxMessage.Data[i] = arr2.s[i-4];
		}
	}
	TxMessage.DLC = sizeof(TxMessage.Data);

	CAN_Transmit(CAN2, &TxMessage);
}

void exampleSendCAN(void){
	TxMessage.StdId = 0x321;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
	
	TxMessage.Data[0]=0x01;
	TxMessage.Data[1]=0x02;
	TxMessage.Data[2]=0x03;
	TxMessage.Data[3]=0x04;
	TxMessage.Data[4]=0x05;
	TxMessage.Data[5]=0x06;
	TxMessage.Data[6]=0x07;
	TxMessage.Data[7]=0x08;
  
	TxMessage.DLC = sizeof(TxMessage.Data);
	
	CAN_Transmit(CAN2, &TxMessage);
}

void exampleCAN2(void){
	TxMessage.StdID = 0x123;
	TxMessage.DLC = 8;
	TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
	
	for(int i=0; i<8; i++){
		TxMessage.Data[i]=(uint8_t) i;
	}
	
	CAN_Transmit(CAN2, &TxMessage);
}

void CAN2_RX0_IRQHandler(void){
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	
	if(RxMessage.StdID == 0x123){
		if(RxMessage.Data[4] == 0xFF)
			;
	}
}

/**
 * @file communicationExample.c
 * @author Simone Bollattino (simone.bollattino@gmail.com)
 * @brief Example with the simpleDataLink.h/.c library
 * 
 * In this example we will simulate an exchange of frames between two
 * nodes, the serial line will be simulated by two circular buffers, like
 * this:
 *  ____________                              ____________
 * |            |----------TxBuff----------->|            |
 * |   NODE 1   |                            |   NODE 2   |
 * |            |<---------RxBuff------------|            |
 * |____________|                            |____________|
 * 
 */

#include "bufferUtils.h"
#include "simpleDataLink.h"
#include <stdio.h>

// buffers declaration
//Tx buffer, this simulates the serial TX line
circular_buffer_handle TxBuff;
uint8_t TxBuffArray[30];
//Rx buffer, this represents the serial RX line
circular_buffer_handle RxBuff;
uint8_t RxBuffArray[30];

//defining txFunc and rxFunc for both nodes
uint8_t txFunc1(uint8_t byte){
	if(!cBuffPushToFill(&TxBuff,&byte,1,1)) return 0;
	return 1;
}
uint8_t rxFunc1(uint8_t* byte){
	if(!cBuffPull(&RxBuff,byte,1,0)) return 0;
	return 1;
}
uint8_t txFunc2(uint8_t byte){
	if(!cBuffPushToFill(&RxBuff,&byte,1,1)) return 0;
	return 1;
}
uint8_t rxFunc2(uint8_t* byte){
	if(!cBuffPull(&TxBuff,byte,1,0)) return 0;
	return 1;
}

//serial line handle structs for both nodes
serial_line_handle line1;
serial_line_handle line2;

int main(){
	//init buffers
	cBuffInit(&TxBuff,TxBuffArray,sizeof(TxBuffArray),0);
	cBuffInit(&RxBuff,RxBuffArray,sizeof(RxBuffArray),0);

	//init lines
	sdlInitLine(&line1,&txFunc1,&rxFunc1);
	sdlInitLine(&line2,&txFunc2,&rxFunc2);

	/*
		We will simulate a loopbak between node 1 and node 2
		Node 1 will send some payloads to node 2 and the latter
		will send them back to node 1
	*/

	char pay1[]="Hello ";
	char pay2[]=" World!";

	//node 1 sends both payloads
	sdlSend(&line1,(uint8_t*)pay1,sizeof(pay1));
	printf("Line 1, sent: %s\n",pay1);
	sdlSend(&line1,(uint8_t*)pay2,sizeof(pay2));
	printf("Line 1, sent: %s\n",pay2);

	//node 2 receives both payloads and sends them back
	char rxPay[20];
	uint32_t num=sdlReceive(&line2,(uint8_t*)rxPay,sizeof(rxPay));
	printf("Line 2, received (%u): %s\n",num,rxPay);
	sdlSend(&line2,(uint8_t*)rxPay,num);
	printf("Line 2, sent back: %s\n",rxPay);
	num=sdlReceive(&line2,(uint8_t*)rxPay,sizeof(rxPay));
	printf("Line 2, received (%u): %s\n",num,rxPay);
	sdlSend(&line2,(uint8_t*)rxPay,num);
	printf("Line 2, sent back: %s\n",rxPay);

	//node 1 receives back
	num=sdlReceive(&line1,(uint8_t*)rxPay,sizeof(rxPay));
	printf("Line 1, received (%u): %s\n",num,rxPay);
	num=sdlReceive(&line1,(uint8_t*)rxPay,sizeof(rxPay));
	printf("Line 1, received (%u): %s\n",num,rxPay);
}
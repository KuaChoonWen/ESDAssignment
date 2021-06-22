/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Mutex Example

*----------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "uart.h"
#include "test.h"


void Producer (void const *argument);
void Consumer1 (void const *argument);
void Consumer2 (void const *argument);
void ShowMes (void const *argument);

osThreadDef(Producer, osPriorityNormal, 1, 0);
osThreadDef(Consumer1, osPriorityNormal, 1, 0);
osThreadDef(Consumer2, osPriorityNormal, 1, 0);
osThreadDef(ShowMes, osPriorityNormal, 1, 0);

osThreadId T_pro;
osThreadId T_con1;
osThreadId T_con2;
osThreadId T_show;
	
osMessageQId Q_LED;
osMessageQDef (Q_LED,0x16,unsigned char);
osEvent  result;
	
osMutexId x_mutex;
osMutexDef(x_mutex);
osSemaphoreId SEMitem;                         // Semaphore ID
osSemaphoreDef(SEMitem);                       // Semaphore definition
osSemaphoreId SEMspace;                         // Semaphore ID
osSemaphoreDef(SEMspace);                       // Semaphore definition

const unsigned int N = 8;
unsigned char buffer[N];
unsigned int tail = 0;
unsigned int head = 0;

//Put item into buffer
void put(unsigned char put_item){
	osSemaphoreWait(SEMspace, osWaitForever);
	osMutexWait(x_mutex, osWaitForever);
		buffer[tail] = put_item;
		tail = (tail + 1) % N;
	osMutexRelease(x_mutex);
	osSemaphoreRelease(SEMitem);
}

//Take item from buffer
unsigned char get(){
	unsigned char take_item=0x00;
	osSemaphoreWait(SEMitem, osWaitForever);
	osMutexWait(x_mutex, osWaitForever);
		take_item = buffer[head];
		buffer[head] = 0x00;
		head = (head + 1) % N;
	osMutexRelease(x_mutex);
	osSemaphoreRelease(SEMspace);
	return take_item;
}

int i = 0;
int j = 0;
int k = 0;
int loopcount = 20;

//Producer
void Producer (void const *argument) {
	char *name = test();
	for(i=0; i<loopcount; i++){
		put(name[i]);
		if (name[i]==0x0A)
			break;
	}
}

//Consumer 1: Get word
void Consumer1 (void const *argument) {
	for(j=0; j<loopcount; j++){
		char data = get();
		osMessagePut(Q_LED,data,osWaitForever);
		if(data==0x0A)
			break;
	}
}

//Consumer 2: Get Word
void Consumer2 (void const *argument) {
	for(k=0; k<loopcount; k++){
		char data2 = get();
		osMessagePut(Q_LED,data2,osWaitForever);
		if(data2==0x0A)
			break;
	}
}

int n = 0;
//Read from Message Queue, Compare and Print Result
void ShowMes (void const *argument){
	char *text = test();
	char cap[8];
	char input[13];
	char output[13];
	char comp[10];
	int size;

	//Input
	sprintf(cap, "Input: ");
	for(n=0; n<7; n++){
		SendChar(cap[n]);
	}
	for(n=0; n<loopcount; n++){
		SendChar(text[n]);
		input[n]=text[n];
		if (input[n] == 0x0A)
			break;
	}
	
	//Output
	sprintf(cap, "Output: ");
	for(n=0; n<8; n++){
		SendChar(cap[n]);
	}
	for(n=0; n<loopcount; n++){
		result = osMessageGet(Q_LED,osWaitForever);	
		output[n] = result.value.v;
		SendChar(result.value.v);
		if (result.value.v == 0x0A)
			break;
	}
	
	size = sizeof(input);
	//Compare
	for(n=0; n<size; n++){
		if (input[n] != output[n]){
			sprintf(comp, " NOT MATCH");
			break;
		}
		else
			sprintf(comp, "MATCH");
	}
	
	//Result
	sprintf(cap, "Result: ");
	for(n=0; n<8; n++){
		SendChar(cap[n]);
	}
	for(n=0; n<10; n++){
		SendChar(comp[n]);
	}
}

int main (void) {
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	USART1_Init();
	SEMitem = osSemaphoreCreate(osSemaphore(SEMitem), 0);
	SEMspace = osSemaphoreCreate(osSemaphore(SEMspace), N);
	x_mutex = osMutexCreate(osMutex(x_mutex));							
	Q_LED = osMessageCreate(osMessageQ(Q_LED),NULL);				//create the message queue
	T_pro = osThreadCreate(osThread(Producer), NULL);				//producer
	T_con1 = osThreadCreate(osThread(Consumer1), NULL);			//consumer1
	T_con2 = osThreadCreate(osThread(Consumer2), NULL);			//consumer2
	T_show = osThreadCreate(osThread(ShowMes), NULL);				//Show Result
	
	osKernelStart ();                         // start thread execution 
}


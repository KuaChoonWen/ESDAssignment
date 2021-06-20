/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Mutex Example

*----------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "cmsis_os.h"
#include "uart.h"
#include "test.h"

void Producer (void const *argument);
void Consumer (void const *argument);
osThreadDef(Producer, osPriorityNormal, 1, 0);
osThreadDef(Consumer, osPriorityNormal, 1, 0);

osThreadId T_pro;
osThreadId T_con;
	
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
int loopcount = 20;

//Producer
void Producer (void const *argument) {
	
	char *name1 = test1();
	char *name2 = test2();
	for(;;)
	{
		for(i=0; i<loopcount; i++){
			put(name1[i]);
			if (name1[i]==0x0A)
				break;
		}
		
		osSignalWait (0x04,osWaitForever);
		
		for(i=0; i<loopcount; i++){
			put(name2[i]);
			if (name2[i]==0x0A)
				break;
		}
		
		osSignalWait (0x04,osWaitForever);
	}
}

//Consumer
void Consumer (void const *argument) {
	for(;;){
		for(j=0; j<loopcount; j++){
			char data = get();
			SendChar(data);
			if(data==0x0A){
				osSignalSet(T_pro,0x04);
				break;
			}
		}
	}
}

int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	USART1_Init();
	SEMitem = osSemaphoreCreate(osSemaphore(SEMitem), 0);
	SEMspace = osSemaphoreCreate(osSemaphore(SEMspace), N);
	x_mutex = osMutexCreate(osMutex(x_mutex));	
	
	T_con = osThreadCreate(osThread(Consumer), NULL);//consumer	
	T_pro = osThreadCreate(osThread(Producer), NULL);//producer

 
	osKernelStart ();                         // start thread execution 
}


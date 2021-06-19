/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Mutex Example

*----------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "cmsis_os.h"
#include "uart.h"

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

long int x=0;
long int i=0;
long int j=0;

const unsigned int N = 8;
unsigned char buffer[N];
unsigned int insertPtr = 0;
unsigned int removePtr = 0;

void put(unsigned char an_item){
	osSemaphoreWait(SEMspace, osWaitForever);
	osMutexWait(x_mutex, osWaitForever);
	buffer[insertPtr] = an_item;
	insertPtr = (insertPtr + 1) % N;
	osMutexRelease(x_mutex);
	osSemaphoreRelease(SEMitem);
}

unsigned char get(){
	unsigned int rr = 0xff;
	osSemaphoreWait(SEMitem, osWaitForever);
	osMutexWait(x_mutex, osWaitForever);
	rr = buffer[removePtr];
	removePtr = (removePtr + 1) % N;
	osMutexRelease(x_mutex);
	osSemaphoreRelease(SEMspace);
	return rr;
}

int loopcount = 20;

//Producer
char name[] = {'H','I','P','P','O','P','O','T','A','M','U','S'};
void Producer (void const *argument) 
{
	for(; i<loopcount; i++){
		put(name[i]);
	}
}

//Consumer
void Consumer (void const *argument) 
{
	for(; j<loopcount; j++){
		char data = get();
		SendChar(data);
	}
}

int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	USART1_Init();
	SEMitem = osSemaphoreCreate(osSemaphore(SEMitem), 0);
	SEMspace = osSemaphoreCreate(osSemaphore(SEMspace), N);
	x_mutex = osMutexCreate(osMutex(x_mutex));	
	
	T_pro = osThreadCreate(osThread(Producer), NULL);//producer
	T_con = osThreadCreate(osThread(Consumer), NULL);//consumer
 
	osKernelStart ();                         // start thread execution 
}


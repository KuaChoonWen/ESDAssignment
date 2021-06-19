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

const unsigned int N = 8;
unsigned char buffer[N];
unsigned int tail = 0;
unsigned int head = 0;

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
char name1[] = {'L','I','O','N','\n'};
char name2[] = {'H','I','P','P','O','P','O','T','A','M','U','S'};

//Producer
void Producer (void const *argument) 
{
	for(;;)
	{
	for(i=0 <loopcount; i++){
		put(name1[i]);
	}
		for(i= 0<loopcount; i++){
		put(name2[i]);
	}
	}
}
//Consumer
void Consumer (void const *argument) 
{
	unsigned int data = 0x00;
	for(; j<loopcount; j++){
		data = get();
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
	
	T_con = osThreadCreate(osThread(Consumer), NULL);//consumer	
	T_pro = osThreadCreate(osThread(Producer), NULL);//producer

 
	osKernelStart ();                         // start thread execution 
}


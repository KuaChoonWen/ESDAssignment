#include "stm32f10x.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "uart.h"


char *test1(){
	char *name = malloc(5*(sizeof(char)));
	name[0] = 'L';
	name[1] = 'I';
	name[2] = 'O';
	name[3] = 'N';
	name[4] = '\n';
	return name;
}

char *test2(){
	char *name = malloc(13*(sizeof(char)));
	name[0] = 'H';
	name[1] = 'I';
	name[2] = 'P';
	name[3] = 'P';
	name[4] = 'O';
	name[5] = 'P';
	name[6] = 'O';
	name[7] = 'T';
	name[8] = 'A';
	name[9] = 'M';
	name[10] = 'U';
	name[11] = 'S';
	name[12] = '\n';
	return name;
}

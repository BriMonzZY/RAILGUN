#include "senser.h"


/* º§π‚≤‚æ‡∑¢ÀÕ÷∏¡Ó */

#if 0

void Get_Distance(void)
{
	printf("get");
	int i;
	for(i = 0;i < 8; i++) {
		HAL_UART_Transmit(&huart2, &CMD_5[i], 1, 10);
	}
}

#endif

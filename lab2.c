/*
 * lab2.c
 *
 *  Created on: 22-09-2020
 *      Author: Chan Kai Yan
 *      SID¡G1155126922
 *      E-mail¡G1155126922@link.cuhk.edu.hk
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

uint8_t ui8PinData=2;
int32_t ButtonState = 0;

int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	while(1)
	{
		ButtonState = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
		if (ButtonState == GPIO_PIN_4) {
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData);
			SysCtlDelay(2000000);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
			SysCtlDelay(2000000);
			if(ui8PinData==8) {ui8PinData=2;} else {ui8PinData=ui8PinData*2;}
		}
		else {
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
			SysCtlDelay(2000000);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 14); //white
			SysCtlDelay(2000000);
		}
	}
}

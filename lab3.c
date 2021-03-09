/*
 * main.c
 *
 *  Created on: 29-9-2020
 *      Author: Chan Kai Yan
 *      SID: 1155126922
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

void GPIOHandler(void);
int32_t ButtonState = 0;
uint8_t ui8PinData=2;

int main(void)
{
	//uint32_t ui32Period;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//change it to gpio interrupt

	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntRegister(GPIO_PORTF_BASE, GPIOHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);

	while(1)
	{
	}
}


void GPIOHandler(void)
{

	// Read the current state of the GPIO pin and
	// write back the opposite state
	ButtonState = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
			if (ButtonState == GPIO_PIN_4) {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData);
				SysCtlDelay(2000000);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
				SysCtlDelay(2000000);
				if(ui8PinData==8) {ui8PinData=2;} else {ui8PinData=ui8PinData*2;}
			}
			else {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 14);
				SysCtlDelay(2000000);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0); //white
				SysCtlDelay(2000000);
			}
}


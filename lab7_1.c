#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"

#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

int main(void)
{
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	char temp[16]={'T','e','m','p','=',' ',' ',' ','C',' ','o','r',' ',' ',' ','F'};
	while(1)
	{
	   ADCIntClear(ADC0_BASE, 1);
	   ADCProcessorTrigger(ADC0_BASE, 1);

	   while(!ADCIntStatus(ADC0_BASE, 1, false))
	   {
	   }

	   ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
	   ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
	   ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
	   ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
	   char i = (ui32TempValueC / 10) + '0';
	   char j = (ui32TempValueC % 10) + '0';
	   temp[6] = i;
	   temp[7] = j;

	   char x = (ui32TempValueF / 10) + '0';
	   char y = (ui32TempValueF % 10) + '0';
	   temp[13] = x;
	   temp[14] = y;

	   int a = 0;
	   while (a < 16){
	   UARTCharPutNonBlocking(UART0_BASE,temp[a]);
	   a++;
	   }

	   SysCtlDelay(2000);
	}
}

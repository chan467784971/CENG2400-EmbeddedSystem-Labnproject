#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"

#define NEWLINE UARTCharPut(UART0_BASE, 0x0a); UARTCharPut(UART0_BASE, 0x0d)
#define NEWLINE1 UARTCharPut(UART1_BASE, 0x0a); UARTCharPut(UART1_BASE, 0x0d)

#define GPIO_PA0_U0RX 	0x00000001
#define GPIO_PA1_U0TX 	0x00000401
#define GPIO_PB0_U1RX 	0x00010001
#define GPIO_PB1_U1TX 	0x00010401
#define GPIO_PE0_U7RX 	0x00040001
#define GPIO_PE1_U7TX 	0x00040401

int i,j,s;

void initSystem(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    // Enable UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Initialize UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    GPIOPinConfigure(GPIO_PE0_U7RX);
    GPIOPinConfigure(GPIO_PE1_U7TX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // configure SW1 and SW2 GPIOF PF0 & PF4
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3|GPIO_PIN_4);     // configure 3,4 as output (A,B)
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_3);     // configure PB6,7 as output (C,D)
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3); // configure PD2 and PD3 as output pins
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4|GPIO_PIN_5); // configure PE4 and PE5 as output pins

    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
}

void delayMS(int ms) {
    SysCtlDelay((SysCtlClockGet()/1000)*ms);
}

int  print(char *p) {
    while(*p!='\0') {
        UARTCharPut(UART0_BASE, (*p++));;
   }
   return(0);
}

int  print1(char *p) {
    while(*p!='\0') {
        UARTCharPut(UART1_BASE, (*p++));;
   }
   return(0);
}

int  print7(char *p) {
    while(*p!='\0') {
        UARTCharPut(UART7_BASE, (*p++));;
   }
   return(0);
}

void putint(int count) {
    UARTCharPut(UART0_BASE,'0' + (count/100) % 10);
    UARTCharPut(UART0_BASE,'0' + (count/10) % 10);
    UARTCharPut(UART0_BASE,'0' + count % 10);
}

void putint1(int count) {
    UARTCharPut(UART1_BASE,'0' + (count/100) % 10);
    UARTCharPut(UART1_BASE,'0' + (count/10) % 10);
    UARTCharPut(UART1_BASE,'0' + count % 10);
}

void putint7(int count) {
    UARTCharPut(UART7_BASE,'0' + (count/100) % 10);
    UARTCharPut(UART7_BASE,'0' + (count/10) % 10);
    UARTCharPut(UART7_BASE,'0' + count % 10);
}

void wait_for_base_ready(void) {
    char cin;

    cin=0;
    while(cin!='S') cin=UARTCharGet(UART7_BASE);
    while(cin!='I') cin=UARTCharGet(UART7_BASE);
}

void avoid_obstacle(){
	print7("l90");
    wait_for_base_ready();

	print7("f030");
    wait_for_base_ready();

	print7("r90");
    wait_for_base_ready();

	print7("f040");
    wait_for_base_ready();

	print7("r90");
    wait_for_base_ready();

	print7("f030");
    wait_for_base_ready();

	print7("l90");
    wait_for_base_ready();


}

int main(void)
{
    char v;

    s=0;
    initSystem();

    NEWLINE;
    print("=============================================================="); NEWLINE;
    print("*               CUHK CSE CENG2400 (2020)                     *"); NEWLINE;
    print("*                                                            *"); NEWLINE;
    print("*                    Test Program                            *"); NEWLINE;
    print("=============================================================="); NEWLINE;
    NEWLINE;

    NEWLINE1;
    print1("=============================================================="); NEWLINE1;
    print1("*               CUHK CSE CENG2400 (2020)                     *"); NEWLINE1;
    print1("*                                                            *"); NEWLINE1;
    print1("*                    Test Program                            *"); NEWLINE1;
    print1("=============================================================="); NEWLINE1;
    NEWLINE1;

    wait_for_base_ready();

    for(i=0;i<19;i++){
        v=GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6);
        if(v==0) {
            avoid_obstacle();
            i+=4;
        }
        print7("f010");
        wait_for_base_ready();
        s++;
    }
}







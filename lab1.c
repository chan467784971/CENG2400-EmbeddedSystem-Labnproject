#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define datapins GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define RS GPIO_PIN_5
#define RW GPIO_PIN_6
#define EN GPIO_PIN_7

void delayMs(int n);
void delayUs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_position(int hor_x, int ver_y);
void LCD_display_message(char *message_str);

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, datapins);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, RS|RW|EN);

    LCD_init();

    while(1)
    {
        LCD_position(0,1);
        LCD_display_message("CSE CENG2400 Tiva C");
        LCD_position(0,2);
        LCD_display_message(" LaunchPad (2020)   ");
        delayMs(50);
    }
}

void delayMs(int n)
{
    int i,j;
    for(i = 0;i < n;i++)
        for(j = 0;j < 3180; j++)
            {}
}

void delayUs( int n)
{
    int i,j;
    for(i = 0;i < n;i++)
        for(j = 0;j < 3; j++)
            {}
}

void LCD_init(void)
{
    delayMs(20); /* initialization sequence */
	LCD_command(0x30);
	delayMs(50);
	LCD_command(0x30);
	delayUs(500);
	LCD_command(0x30);

	LCD_command(0x38); // set 8-bit data, 2-line, 5x7 font
    	LCD_command(0x06); // cursor move direction: increase
   	 LCD_command(0x01); // display clear
   	 LCD_command(0x02); // cursor move to first digit
	LCD_command(0x0F); //turn on display, cursor blinking
}

void LCD_command(unsigned char command)
{
    GPIOPinWrite(GPIO_PORTA_BASE, RS, 0x00); //RS = 0->command mode
    GPIOPinWrite(GPIO_PORTA_BASE, RW, 0x00); //RW = 0->writing mode
    GPIOPinWrite(GPIO_PORTB_BASE, datapins, command); // Write the command to data pins
    GPIOPinWrite(GPIO_PORTA_BASE, EN, 0x80); // Set EN to high
    delayUs(500);
    GPIOPinWrite(GPIO_PORTA_BASE, EN, 0x00); // Set EN to low, a high-to-low pulse
	if (command < 4)
		delayMs(20); /* command 1 and 2 needs up to 1.64ms */
	else
		delayUs(400); /* all others 40 us */
}

void LCD_data(unsigned char data)
{
    GPIOPinWrite(GPIO_PORTA_BASE, RS, 0x20); //RS = 1->data mode
    GPIOPinWrite(GPIO_PORTA_BASE, RW, 0x00); //RW = 0->writing mode
    GPIOPinWrite(GPIO_PORTB_BASE, datapins, data); // Write the command to data pins
    GPIOPinWrite(GPIO_PORTA_BASE, EN, 0x80); // Set EN to high
    delayUs(500);
    GPIOPinWrite(GPIO_PORTA_BASE, EN, 0x00); // Set EN to low, a high-to-low pulse
}

void LCD_position(int hor_x, int ver_y) {
        switch(ver_y) {
    case 1:
                LCD_command(0x80+hor_x);
                delayMs(50);
        break;
    case 2:
                LCD_command(0xc0+hor_x);
                delayMs(50);
        break;
    case 3:
                LCD_command(0x94+hor_x);
                delayMs(50);
        break;
    case 4:
                LCD_command(0xd4+hor_x);
                delayMs(50);
        break;
        }
}

void LCD_display_message(char *message_str) {
    int n;

    n=0;
    while (message_str[n]!='\0') {
        LCD_data(message_str[n]);
        n++;
    }
}

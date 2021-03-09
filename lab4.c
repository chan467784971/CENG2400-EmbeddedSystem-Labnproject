/*
 * lab4.c
 *
 *  Created on: 10-10-2020
 *      Author: Chan Kai Yan
 */


/*Sorry for the program cannot function well
 * it can't generate lab4.out file and output it to device, so the video is showing the function that can show messages
 * (but in  while loop, so it is flashing)
 *
 * The reason that i find it can't generate .out file coz it can't built, so i try to build and get following error messages
 *    At end of source: warning #994-D: missing return statement at end of non-void function "keypad_getkey"
 *    At end of source: error #68: expected a "}"
 * However, no } was missing and no return is missing, even not error msg on problems column (at right bottom )
 * but it can't run ¡K¡K
 * so horrible
 * */
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
//#define ROW GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|
//#define COL GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|

void delayMs(int n);
void delayUs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_position(int hor_x, int ver_y);
void LCD_display_message(char *message_str);

void keypad_init(void);
unsigned char keypad_getkey(void);


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, datapins);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, RS|RW|EN);

    unsigned char key;
    char defaultpw[10];
    char enterpw[10];

    keypad_init();
    LCD_init();

    while(1)
    {
        LCD_position(0,1);
        LCD_display_message("Chan Kai Yan");
        LCD_position(0,2);
        LCD_display_message(" 1155126922  ");
        delayMs(50);


        //read the key and show
        LCD_position(0,1);
        LCD_display_message("Please press the key:");
        LCD_position(0,2);
        delayMs(20);
        LCD_command(0x80);
        key = keypad_getkey();
        if(key != 0)
        LCD_data(key);  //display the key

        delayMs(20);



        //setting password
        LCD_position(0,1);
        LCD_display_message("Please set the pw:");
        LCD_position(0,2);
        int i = 0;
        while(key != '#'){
        key = keypad_getkey();
        LCD_data(key);
        defaultpw[i] = key; //save password in char[]
        i++;
        }
        delayMs(20);


        //asking enter password and check
        LCD_position(0,1);
        LCD_display_message("Please enter the pw:");
        LCD_position(0,2);
        int j = 0;
        while(key != '#'){
        key = keypad_getkey();
        LCD_data(key);
        enterpw[j] = key;
        j++;
        }

        //diff. length->must wrong
        if (i != j){
        LCD_position(0,1);
        LCD_display_message("Wrong! try again:");
        }

        //check the password digit 1 by 1
        int k = 0;
        while (k < i){
        	if(defaultpw[k] != enterpw[k]){//wrong password
                LCD_position(0,1);
                LCD_display_message("Wrong! try again:");
        	}
        	k++;
        }
        //all match->correct
        LCD_position(0,1);
        LCD_display_message("Correct! enter again:");

        delayMs(20);

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

void keypad_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

//	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, ROW); //output, row
	GPIO_PORTE_DEN_R |= 0x0F;
	GPIO_PORTE_DIR_R |= 0x0F;//row 0-3
	GPIO_PORTE_ODR_R |= 0x0F;//open drain


//	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, COL);//input, col
	GPIO_PORTE_DEN_R |= 0x0F;
	GPIO_PORTE_DIR_R &= ~0x0F;//col 4-7
	GPIO_PORTE_PUR_R |= 0xF0;//pull-up

}


unsigned char keypad_getkey(void){
	const unsigned char keymap[4][3] = {
	{ '1', '2', '3'},
	{ '4', '5', '6'},
	{ '7', '8', '9'},
	{ '*', '0', '#'},
};

	// check to see any key pressed
	int row, col;
	GPIO_PORTE_DATA_R = 0; //all row
	col = GPIO_PORTC_DATA_R & 0xF0;
	if (col == 0xF0) return 0;

	while (1)
	{
		row = 0;
		GPIO_PORTE_DATA_R = 0x0E; //row0
		delayUs(2);
		col = GPIO_PORTC_DATA_R & 0xF0;
		if (col != 0xF0) break;

		row = 1;
		GPIO_PORTE_DATA_R = 0x0D; //row1
		delayUs(2);
		col = GPIO_PORTC_DATA_R & 0xF0;
		if (col != 0xF0) break;

		row = 2;
		GPIO_PORTE_DATA_R = 0x0B;//row3
		delayUs(2);
		col = GPIO_PORTC_DATA_R & 0xF0;
		if (col != 0xF0) break;

		row = 3;
		GPIO_PORTE_DATA_R = 0x07;//row4
		delayUs(2);
		col = GPIO_PORTC_DATA_R & 0xF0;
		if (col != 0xF0) break;

	if (col == 0xE0) return keymap[row][0]; // key in column 0
	if (col == 0xD0) return keymap[row][1]; // key in column 1
	if (col == 0xB0) return keymap[row][2]; // key in column 2
	return 0;
}

/*
 * lab5.c
 *
 *  Created on: 18-10-2020
 *      Author: Chan Kai Yan
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

#define datapins GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define RS GPIO_PIN_5
#define RW GPIO_PIN_6
#define EN GPIO_PIN_7
#define ROW GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
#define COL GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6

void delayMs(int n);
void delayUs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_position(int hor_x, int ver_y);
void LCD_display_message(char *message_str);

void keypad_init(void);
unsigned char keypad_getkey(void);

void GPIOHandler(void);

int main(void)
	{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, datapins);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, RS|RW|EN);

    //ROW and COL setting
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, ROW);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, COL);
    GPIOPadConfigSet(GPIO_PORTC_BASE, COL,GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);



    //GPIO interrupt switch 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntRegister(GPIO_PORTF_BASE, GPIOHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);



	while(1){
		LCD_init();
	    char defaultpw[10]={'7','8','9','#','n','n','n','n','n','n'};
	    char enterpw[10]={'n','n','n','n','n','n','n','n','n','n'};
	    char key;

	    //CHECK PW

	    LCD_position(0,1);
	    LCD_display_message("Please enter the pw:");
	    LCD_position(0,2);
	    LCD_display_message("                   ");
	    key = 'n';
	    delayMs(500);

	 	while(1)
	 	{
	    LCD_position(0,2);
	    int j = 0;
	    	while(key != '#'){
	    		while(key=='n'){
	    			key = keypad_getkey();
	    		}
	    		delayMs(100);
	    		LCD_data(key);
	    		enterpw[j] = key;
	    		j++;
	    		if(key != '#'){
	    			key = 'n';
	    		}
	    		delayMs(500);
	    }
	    delayMs(100);


	    //diff. length->must wrong
	    if (j != 4){
	    LCD_position(0,1);
	    LCD_display_message("Wrong! try again:");
	    delayMs(500);
	    }

	    //check the password digit 1 by 1
	    int k = 0;
	    while (k < 4){
	    	if(defaultpw[k] != enterpw[k]){  //wrong password
	            LCD_position(0,1);
	            LCD_display_message("Wrong! try again:");
	            delayMs(500);

	    	}
	    	k++;
	    }
	    //all match->correct
	    LCD_position(0,1);
	    LCD_display_message("Correct! enter again:");
	    delayMs(500);


	}



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

/*
void keypad_init(void){
	SYSCTL_RCGCGPIO_R |= 0x14;

//	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, ROW); //output, row
	GPIO_PORTE_DEN_R |= 0x0F;
	GPIO_PORTE_DIR_R |= 0x0F;//row 0-3
	GPIO_PORTE_ODR_R |= 0x0F;//open drain


//	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, COL);//input, col
	GPIO_PORTC_DEN_R |= 0xF0;
	GPIO_PORTC_DIR_R &= ~0xF0;//col 4-7
	GPIO_PORTC_PUR_R |= 0xF0;//pull-up

}
*/

unsigned char keypad_getkey(void){
	GPIOPinWrite(GPIO_PORTE_BASE,ROW,0x1C);  //ROW 0
	if(!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)){
		return '1';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_5)){
		return '2';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6)){
		return '3';
	}

	GPIOPinWrite(GPIO_PORTE_BASE,ROW,0x1A);  //ROW 1
	if(!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)){
		return '4';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_5)){
		return '5';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6)){
		return '6';
	}

	GPIOPinWrite(GPIO_PORTE_BASE,ROW,0x16);  //ROW 2
	if(!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)){
		return '7';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_5)){
		return '8';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6)){
		return '9';
	}

	GPIOPinWrite(GPIO_PORTE_BASE,ROW,0x0E);  //ROW 3
	if(!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)){
		return '*';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_5)){
		return '0';
	}
	else if (!GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_6)){
		return '#';
	}

return 'n';
}


void GPIOHandler(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntRegister(GPIO_PORTF_BASE, GPIOHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);


	LCD_init();
    char defaultpw[10]={'n','n','n','n','n','n','n','n','n','n'};
    char enterpw[10]={'n','n','n','n','n','n','n','n','n','n'};
    char key;

    key = 'n';

    //setting password
    LCD_position(0,1);
    LCD_display_message("Please set the pw:");
    LCD_position(0,2);
    int i = 0;
    while(key != '#'){
    	while (key == 'n'){
    		key = key = keypad_getkey();
    	}
    	delayMs(100);
    	LCD_data(key);
    	defaultpw[i] = key; //save password in char[]
    	i++;
    	if(key != '#'){
    	key = 'n';
    	}
    	delayMs(500);
    }
    delayMs(500);

    //CHECK PW
    inputpw:
    LCD_position(0,1);
    LCD_display_message("Please enter the pw:");
    LCD_position(0,2);
    LCD_display_message("                   ");
    key = 'n';
    delayMs(500);

 	while(1)
 	{
    LCD_position(0,2);
    int j = 0;
    	while(key != '#'){
    		while(key=='n'){
    			key = keypad_getkey();
    		}
    		delayMs(100);
    		LCD_data(key);
    		enterpw[j] = key;
    		j++;
    		if(key != '#'){
    			key = 'n';
    		}
    		delayMs(500);
    }
    delayMs(100);


    //diff. length->must wrong
    if (i != j){
    LCD_position(0,1);
    LCD_display_message("Wrong! try again:");
    delayMs(500);
    }

    //check the password digit 1 by 1
    int k = 0;
    while (k < i){
    	if(defaultpw[k] != enterpw[k]){  //wrong password
            LCD_position(0,1);
            LCD_display_message("Wrong! try again:");
            delayMs(500);
            goto inputpw;
    	}
    	k++;
    }
    //all match->correct
    LCD_position(0,1);
    LCD_display_message("Correct! enter again:");
    delayMs(500);
    goto inputpw;


    delayMs(20);

}


}




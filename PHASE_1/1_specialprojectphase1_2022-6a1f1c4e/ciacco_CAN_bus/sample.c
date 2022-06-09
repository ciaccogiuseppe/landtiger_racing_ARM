/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT buttons and manage the bouncing effect
 *        	- key1 switches on the led at the left of the current led on, 
 *					- it implements a circular led effect. 	
  * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
//#include "timer/timer.h"
#include "RIT/RIT.h"
#include "GLCD/GLCD.h" 
#include "CAN/CAN.h"
#include "timer/timer.h"
#include "joystick/joystick.h"

/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in funct_led								*/
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


void can_Init (void) {

  CAN_setup (1);                                  /* setup CAN Controller #1  */
  CAN_setup (2);                                  /* setup CAN Controller #2  */

  //CAN_wrFilter (1, 0, STANDARD_FORMAT);          /* Enable reception of msgs */
	//CAN_wrFilter (2, 0, STANDARD_FORMAT);          /* Enable reception of msgs */

  CAN_start (1);                                  /* start CAN Controller #1  */
  CAN_start (2);                                  /* start CAN Controller #2  */

  CAN_waitReady (1);                              /* wait til tx mbx is empty */
  CAN_waitReady (2);                              /* wait til tx mbx is empty */
}



/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	int i;
	//int tick = 20000;
	//int val_Tx = 12;
	//char c[124] = {0};
	//int val_Rx;
	//char str[128] = "";
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  BUTTON_init();												/* BUTTON Initialization              */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	
	joystick_init();
	
	enable_RIT();
	
	
	LCD_Initialization();
	LCD_Clear(Black);
	GUI_Text(0, 0, (uint8_t*)" CAN BUS TEST ", White, Red);
	
	
	can_Init();
	
	CAN_TxMsg[0].id = 0;                           /* initialize msg to send   */
  for (i = 0; i < 8; i++) CAN_TxMsg[0].data[i] = 0;
  CAN_TxMsg[0].len = 1;
  CAN_TxMsg[0].format = STANDARD_FORMAT;
  CAN_TxMsg[0].type = DATA_FRAME;
	
	CAN_TxMsg[1].id = 0;                           /* initialize msg to send   */
  for (i = 0; i < 8; i++) CAN_TxMsg[1].data[i] = 0;
  CAN_TxMsg[1].len = 1;
  CAN_TxMsg[1].format = STANDARD_FORMAT;
  CAN_TxMsg[1].type = DATA_FRAME;
	

	init_timer(0, 0x0000B400);
	init_timer(1, 0x000CB735);									/* RIT Initialization 50 msec       */
	
	init_timer(2, 0x017D7840);
	
	enable_timer(0);
	enable_timer(1);
	
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);				
  while (1)	
  {
		__ASM("wfi");
  }

}

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

/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in funct_led								*/
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


void can_Init (void) {

  CAN_setup (1);                                  /* setup CAN Controller #1  */
  CAN_setup (2);                                  /* setup CAN Controller #2  */

  CAN_wrFilter (1, 33, STANDARD_FORMAT);          /* Enable reception of msgs */
	CAN_wrFilter (2, 33, STANDARD_FORMAT);          /* Enable reception of msgs */

  CAN_start (1);                                  /* start CAN Controller #1  */
  CAN_start (2);                                  /* start CAN Controller #2  */

  CAN_waitReady (1);                              /* wait til tx mbx is empty */
  CAN_waitReady (2);                              /* wait til tx mbx is empty */
}



/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	int i, sw1 = 0;
	//int tick = 20000;
	//int val_Tx = 12;
	char c[124] = {0};
	//int val_Rx;
	char str[128] = "";
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  BUTTON_init();												/* BUTTON Initialization              */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	
	enable_RIT();
	
	
	LCD_Initialization();
	LCD_Clear(Black);
	GUI_Text(0, 0, (uint8_t*)" CAN BUS TEST ", White, Red);
	
	
	can_Init();
	
	CAN_TxMsg[0].id = 33;                           /* initialize msg to send   */
  for (i = 0; i < 8; i++) CAN_TxMsg[0].data[i] = 0;
  CAN_TxMsg[0].len = 1;
  CAN_TxMsg[0].format = STANDARD_FORMAT;
  CAN_TxMsg[0].type = DATA_FRAME;
	
	CAN_TxMsg[1].id = 33;                           /* initialize msg to send   */
  for (i = 0; i < 8; i++) CAN_TxMsg[1].data[i] = 0;
  CAN_TxMsg[1].len = 1;
  CAN_TxMsg[1].format = STANDARD_FORMAT;
  CAN_TxMsg[1].type = DATA_FRAME;
	

	init_timer(0, 0x0000B400);
	enable_timer(0);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
	while(1){
		CAN_receiveBuffers(2, c, 124); 			//change depending on phyisical link
		
		sprintf(str, "");
		for(i=0; i< 124; i++)
		{
			sprintf(str, "%s%c", str, c[i]);
			//t[i]=c[i];
		}
		sprintf(str, "%s%c", str, 0);
		CAN_transmitBuffers(1,c,124);				//change depending on phyisical link
		
		if(sw1 == 0){
				GUI_Text(0, 120, (uint8_t*)str, White, Red);
				sw1 = 1;
			}
			else{
				GUI_Text(0, 120, (uint8_t*)str, White, Blue);
				sw1 = 0;
			}
		
		
//		if (CAN_RxRdy[0]) {                           /* rx msg on CAN Ctrl #1    */
//			CAN_RxRdy[0] = 0;
//			val_Rx = CAN_RxMsg[0].data[0];
//			sprintf(str, "");
//			for (i = 0; i < CAN_RxMsg[0].len; i++){
//				sprintf(str, "%s|%c|", str, CAN_RxMsg[0].data[i]);
//			}
//			//sprintf(str, "%d", CAN_RxMsg[0].data[0]);
//			
//			GUI_Text(0, 80, (uint8_t*)"                   " , Black, Black);
//			if(sw1 == 0){
//				GUI_Text(0, 60, (uint8_t*)"Received from CAN-1" , White, Red);
//				GUI_Text(0, 120, (uint8_t*)str, White, Red);
//				sw1 = 1;
//			}
//			else{
//				GUI_Text(0, 60, (uint8_t*)"Received from CAN-1" , White, Blue);
//				GUI_Text(0, 120, (uint8_t*)str, White, Blue);
//				sw1 = 0;
//			}
//		}
//		else if (CAN_RxRdy[1]){
//			CAN_RxRdy[1] = 0;
//			val_Rx = CAN_RxMsg[1].data[0];
//			sprintf(str, "%d", CAN_RxMsg[1].data[0]);
//			GUI_Text(0, 60, (uint8_t*)"                   " , Black, Black);
//			if(sw1 == 0){
//				GUI_Text(0, 80, (uint8_t*)"Received from CAN-2" , White, Red);
//				GUI_Text(0, 120, (uint8_t*)str, White, Red);
//				sw1 = 1;
//			}
//			else{
//				GUI_Text(0, 80, (uint8_t*)"Received from CAN-2" , White, Blue);
//				GUI_Text(0, 120, (uint8_t*)str, White, Blue);
//				sw1 = 0;
//			}
//		}
	}
	/*
  while (1)	
  {
		__ASM("wfi");
  }*/

}

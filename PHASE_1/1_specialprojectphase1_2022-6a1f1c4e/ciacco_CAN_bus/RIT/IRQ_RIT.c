/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../CAN/CAN.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int sw1 = 0;
int sw2 = 0;
int down_INT0=0;
int down_KEY1=0;
int down_KEY2=0;


char v[8] = {1,2,3,4,5,6,7,8};
char c[124] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
extern unsigned int  waitForLoop;
extern unsigned char CAN_selfID;
extern char CAN_selected;

void CAN_connection(){
	//char blk;
	if(CAN_selfID == 0)
	{
		CAN_TxMsg[0].id = 0;
		CAN_TxMsg[0].len = 2;
		CAN_TxMsg[0].data[0] = 0;
		CAN_TxMsg[0].data[1] = 0;
		CAN_wrMsg (2, &CAN_TxMsg[0]);               /* transmit message         */
		waitForLoop = 1;
	}
}


void RIT_IRQHandler (void)
{			
		static int up   = 0;	
		static int down = 0;
		static int sel  = 0;
		// 29 up
		// 28 right
		// 27 left
		// 26 down
		// 25 sel
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
			/* Joytick UP pressed */
			up++;
			switch(up){
				case 1:
					CAN_selected++;
					break;
				default:
					break;
			}
		}
		else{
				up=0;
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
			/* Joytick DOWN pressed */
			down++;
			switch(down){
				case 1:
					CAN_selected--;
					break;
				default:
					break;
			}
		}
		else{
				down=0;
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
			/* Joytick SELECT pressed */
			sel++;
			switch(sel){
				case 1:
					if(CAN_selected != 0)
					{
						CAN_TxMsg[0].id = CAN_selected;
						CAN_TxMsg[0].len = 1;
						CAN_TxMsg[0].data[0] = 0;
						CAN_wrMsg (2, &CAN_TxMsg[0]);               /* transmit message         */
						waitForLoop = 1;
					}
					break;
				default:
					break;
			}
		}
		else{
				sel=0;
		}
	
	
		/* KEY1 */
		if(down_KEY1!=0){ 
			if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
				down_KEY1++;				
				switch(down_KEY1){
					case 2:
						if (CAN_TxRdy[0]) {                           /* tx msg on CAN Ctrl #2    */
							CAN_TxRdy[0] = 0;
							CAN_TxMsg[0].data[0] = 12;              /* data[0] = ADC value      */
							CAN_wrMsg (1, &CAN_TxMsg[0]);               /* transmit message         */
							GUI_Text(0,40, (uint8_t*)"          ", Black, Black);
							if(sw1 == 0)
							{
								//GUI_Text(0, 0, "Sent                   ", White, Red);
								GUI_Text(0,20, (uint8_t*)"CAN-1 send", Black, Green);
								sw1 = 1;
							}
							else
							{
								//GUI_Text(0, 0, "Sent                   ", White, Blue);
								GUI_Text(0,20, (uint8_t*)"CAN-1 send", Black, Yellow);
								sw1 = 0;
							}
							enable_timer(2);
						}
						break;
					default:
						break;
				}
			}
			else {	/* button released */
				down_KEY1=0;			
				NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
			}
		}
		
		
		if(down_KEY2!=0){ 
			if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */
				down_KEY2++;				
				switch(down_KEY2){
					case 2:
						if (CAN_TxRdy[1]) {                           /* tx msg on CAN Ctrl #2    */
							CAN_TxRdy[1] = 0;
							CAN_TxMsg[1].data[0] = 13;              /* data[0] = ADC value      */
							CAN_TxMsg[1].data[1] = 32;
							CAN_TxMsg[1].len = 2;
							CAN_wrMsg (2, &CAN_TxMsg[1]);               /* transmit message         */
							GUI_Text(0,20, (uint8_t*)"          ", Black, Black);
							if(sw2 == 0)
							{
								//GUI_Text(0, 0, "Sent                   ", White, Red);
								GUI_Text(0,40, (uint8_t*)"CAN-2 send", Black, Yellow);
								sw2 = 1;
							}
							else
							{
								//GUI_Text(0, 0, "Sent                   ", White, Blue);
								GUI_Text(0,40, (uint8_t*)"CAN-2 send", Black, Green);
								sw2 = 0;
							}
							enable_timer(2);
						}
						break;
					default:
						break;
				}
			}
			else {	/* button released */
				down_KEY2=0;			
				NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
			}
		}
	
		if(down_INT0!=0){ 
			if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* KEY2 pressed */
				down_INT0++;				
				switch(down_INT0){
					case 2:
						//CAN_transmitBuffers (2, c, 124);
							CAN_connection();
							//CAN_transmitBuffers (2, c, 8);
//						if (CAN_TxRdy[1]) {                           /* tx msg on CAN Ctrl #2    */
//							CAN_TxRdy[1] = 0;
//							CAN_TxMsg[1].data[0] = 13;              /* data[0] = ADC value      */
//							CAN_TxMsg[1].data[1] = 32;
//							CAN_TxMsg[1].len = 2;
//							CAN_wrMsg (2, &CAN_TxMsg[1]);               /* transmit message         */
//							GUI_Text(0,20, (uint8_t*)"          ", Black, Black);
//							if(sw2 == 0)
//							{
//								//GUI_Text(0, 0, "Sent                   ", White, Red);
//								GUI_Text(0,40, (uint8_t*)"CAN-2 send", Black, Yellow);
//								sw2 = 1;
//							}
//							else
//							{
//								//GUI_Text(0, 0, "Sent                   ", White, Blue);
//								GUI_Text(0,40, (uint8_t*)"CAN-2 send", Black, Green);
//								sw2 = 0;
//							}
//						}
						break;
					default:
						break;
				}
			}
			else {	/* button released */
				down_INT0=0;			
				NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
			}
		}
	
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

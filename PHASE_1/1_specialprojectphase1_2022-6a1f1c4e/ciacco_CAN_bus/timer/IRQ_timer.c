/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../CAN/CAN.h"

void* timer_buf;
uint32_t timer_len;
uint32_t timer_pos;
uint32_t timer_ctrl;
uint32_t transmitting;

uint32_t can_1_recv=0;
uint32_t can_2_recv=0;

char CAN_selected;

extern unsigned int CAN_TxRdy[2];
extern char CAN_selfID;
char id[124] = {0};
char sel_id[124] = {0};

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern int CAN_active;
void TIMER0_IRQHandler (void)
{
	if(timer_len > 0)
	{
		CAN_transmitBuffer(timer_ctrl, (char*)timer_buf+timer_pos*8, timer_len<8? timer_len:8);
		if(timer_len > 8)
			timer_len-=8;			/*next 8 bytes*/
		else
			timer_len=0;			/*empty buffer*/
		timer_pos+=1;				/*move to next data block (8 byte)*/
	}
	else
	{
		transmitting = 0;		/*set ready for next transmission*/
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	sprintf(id,     " BOARD ID: %3d - USE INT0     ", CAN_selfID);
	sprintf(sel_id, " SEND TO:  %3d - USE JOYSTICK ", CAN_selected);
	if(can_1_recv == 0)
	{
		//GUI_Text(0, 0, "Sent                   ", White, Red);
		GUI_Text(0,150, (uint8_t*)"CAN-1 receive", Black, Yellow);
	}
	else
	{
		//GUI_Text(0, 0, "Sent                   ", White, Blue);
		GUI_Text(0,150, (uint8_t*)"CAN-1 receive", Black, Green);
	}
	
	
	if(can_2_recv == 0)
	{
		//GUI_Text(0, 0, "Sent                   ", White, Red);
		GUI_Text(0,180, (uint8_t*)"CAN-2 receive", Black, Yellow);
	}
	else
	{
		//GUI_Text(0, 0, "Sent                   ", White, Blue);
		GUI_Text(0,180, (uint8_t*)"CAN-2 receive", Black, Green);
	}
	
	GUI_Text(0,300, (uint8_t*)id, Black, Cyan);
	GUI_Text(0,280, (uint8_t*)sel_id, Black, Cyan);
	
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{
	volatile uint32_t icr;
	icr = LPC_CAN1->ICR;
	icr = LPC_CAN2->ICR;
	CAN_TxRdy[0] = 1;
	CAN_TxRdy[1] = 1;
	GUI_Text(0,250, (uint8_t*)"CAN ERROR", Black, Red);
	//disable_timer(2);
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
}


/******************************************************************************
**                            End Of File
******************************************************************************/

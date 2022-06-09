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
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../CAN/CAN.h"

void* timer_buf;
uint32_t timer_len;
uint32_t timer_pos;
uint32_t timer_ctrl;
uint32_t transmitting;

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
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

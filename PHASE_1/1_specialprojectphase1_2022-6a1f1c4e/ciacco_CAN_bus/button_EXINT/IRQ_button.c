#include "button.h"
#include "lpc17xx.h"

//	#include "../led/led.h" 					/* do not needed anymore, make your project clean */
#include "../RIT/RIT.h"		  			/* you now need RIT library 			 */

extern int down_INT0;
extern int down_KEY1;
extern int down_KEY2;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	down_INT0 = 1;
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	//enable_RIT();										/* enable RIT to count 50ms				 */
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	down_KEY1 = 1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	//enable_RIT();										/* enable RIT to count 50ms				 */
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	down_KEY2 = 1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}



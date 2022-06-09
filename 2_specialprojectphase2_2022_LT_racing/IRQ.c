/*----------------------------------------------------------------------------
 * Name:    IRQ.c
 * Purpose: IRQ Handler
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC17xx.h"

unsigned long ticks = 0;
unsigned char ClockLEDOn;               /* On for 30 ms every 50 ms           */
unsigned char ClockLEDOff;              /* On for 20 ms every 50 ms           */
unsigned char ClockANI;                 /* Clock for Animation 150 ms         */
int up = 0;
int down = 0;
int left = 0;
int right = 0;
int sel = 0;
extern short x;
extern short y;
extern unsigned char animActive;
extern unsigned char restart;

/*----------------------------------------------------------------------------
  Systick Interrupt Handler
  SysTick interrupt happens every 10 ms
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) {
	
  ticks++;
  switch (ticks) {
    case  3:
      ClockLEDOff = 1;
			
      break;
    case  5:
      ClockLEDOn  = 1;
      break;
    case  8:
      ClockLEDOff = 1;
      break;
    case 10:
      ClockLEDOn  = 1;
      break;
    case 13:
      ClockLEDOff = 1;
      break;
    case 15:
      ticks       = 0;
      ClockANI    = 1;
      ClockLEDOn  = 1;
    default:
      break;
  }
	
	
	//----------------------------------------------------
	if(ticks % 2 == 0){
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
			/* Joytick UP pressed */
			up++;
			if(x < 200)
				x++;
			/*switch(up){
				case 1:
					x++;
					break;
				default:
					break;
			}*/
		}
		else{
				up=0;
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
			/* Joytick UP pressed */
			left--;
			if(y > 30)
				y--;
			/*switch(up){
				case 1:
					x++;
					break;
				default:
					break;
			}*/
		}
		else{
				left=0;
		}
		
		
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
			/* Joytick UP pressed */
			right++;
			if(y < 150)
				y++;
			/*switch(up){
				case 1:
					x++;
					break;
				default:
					break;
			}*/
		}
		else{
				right=0;
		}
		
		
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
			/* Joytick DOWN pressed */
			down++;
			if(x > 0)
				x--;
			
			/*
			switch(down){
				case 1:
					x--;
					break;
				default:
					break;
			}*/
		}
		else{
				down=0;
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
			/* Joytick DOWN pressed */
			sel++;
			
			
			switch(sel){
				case 1:
					if(animActive == 0){
						
						animActive = 1;
					}
					if(restart == 2){
						restart = 1;
					}
					break;
				default:
					break;
			}
		}
		else{
				sel=0;
		}
		
	}
}

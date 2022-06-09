/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher and Graphic Demo
 * Note(s): 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include "LPC17xx.h"                    /* NXP LPC17xx definitions            */
#include "string.h"
#include <stdlib.h>
#include "./GLCD/GLCD.h"
#include "./LED/LED.h"
#include "./CAN/CAN.h"


#define MAXITEMS 3
#define TRACKLEN 10000 //20000

struct car {
	unsigned short *TEXTURE;
	short x;
	short y;
};

struct item {
	unsigned short *TEXTURE;
	unsigned short *SCORETEXTURE;
	short x;
	short y;
	short score;
	short active;
	short to_signal;
	short collected;
};

enum STATE {
	MENU,
	RUN,
	WAITING,
	END
};

enum ITEM_TYPE{
	POINTP1,
	POINTM1,
	POINTRP2,
	POINTRM2
};



void CAN_InitialMessage(int player){
	int y = player == 1? 100 : 50;
	int i;
	if (CAN_TxRdy[1]) {                           /* tx message on CAN Controller #2 */
					
		CAN_TxRdy[1] = 0;

		CAN_TxMsg[1].data[0] = 50;             /* data[0] field = ADC value */
		CAN_TxMsg[1].data[1] = y;
		for(i = 2; i < 8; i++){
			CAN_TxMsg[1].data[i] = 0;
		}
		
		CAN_wrMsg (2, &CAN_TxMsg[1]);               /* transmit message */
	}
}


extern unsigned char ClockLEDOn;
extern unsigned char ClockLEDOff;
extern unsigned char ClockANI;

extern unsigned short Bg_16bpp_t[];
extern unsigned short BTN_START[];
extern unsigned short LOGO[];
extern unsigned short BGBARS[];
extern unsigned short NUMS[];
extern unsigned short PLAYERS[];
extern unsigned short NUMBERS[];
extern unsigned short WAIT[];
extern unsigned short TRACK[];
extern unsigned short MINICAR[];
extern unsigned short CAR_TEXTURE_RED[];
extern unsigned short CAR_TEXTURE_YELLOW[];
extern unsigned short EMPTYCAR[];
extern unsigned short GAMEOVER[];
extern unsigned short YOUWIN[];
extern unsigned short YOULOSE[];
extern unsigned short DRAW[];
extern unsigned short PLUSONE[];
extern unsigned short MINONE[];
extern unsigned short PLUSTWO[];
extern unsigned short MINTWO[];
extern unsigned short POINT[];
extern unsigned short NPOINT[];
extern unsigned short RANDPOINT[];
extern unsigned short DELETEITEM[];
extern unsigned short NUMBIG[];

unsigned short x = 50;
unsigned short y = 50;
unsigned char animActive;
unsigned char restart = 0;
unsigned int ready;
int score[4];
int prevscore[4];
unsigned int players[4];
unsigned int player = 0;
enum STATE state = MENU;
struct item ITEMS_ON_SCREEN[MAXITEMS];
int km = 0;

/****************** Set values for MENU state *************************/
void GAME_InitialState(){
	int i;
	state = MENU;
	GLCD_Clear(White);
	GLCD_Bmp (  45,   70,  25,  100, BTN_START);
	animActive = 0;
	restart = 0;
	for(i = 0; i < MAXITEMS; i++){
		ITEMS_ON_SCREEN[i].active = 0;
	}
}

/****************** Set values for RUN state **************************/
void GAME_RunningState(){
	GLCD_Clear  (BGCOLOR);
	state = RUN;
	animActive = 0;
}

/****************** Set values for WAIT state *************************/
void GAME_WaitingState(){
	GLCD_Clear  (BGCOLOR);
	state = WAITING;
	animActive = 2;
}

/****************** Set values for END state (1) **********************/
void GAME_GameOverStateFirst(){
	GLCD_Clear(White);
	GLCD_Bmp (  105,   0,  214,  240, GAMEOVER);
	if(score[player-1] < score[2-player]){
		GLCD_Bmp (  20,   59,  22,  122, YOULOSE);
	}
	else if(score[player-1] > score[2-player]){
		GLCD_Bmp (  20,   63,  72,  114, YOUWIN);
	}
	else{
		GLCD_Bmp (  20,   80,  22,   80, DRAW);
	}
}

/****************** Set values for END state (2) **********************/
void GAME_GameOverStateSecond(){
	state = END;
	restart = 2;
	ready = 0;
	km = 0;
}

/****************** Compute log10 of number ***************************/
int log_10(int val){
	int j = 0;
	while (val != 0) {
		val /= 10;
		j+=1;
	}
	return j;
}

/****************** Check if item collides with car *******************/
int GAME_ItemCollides(struct item i, struct car c){
	return 	i.x + 16 > c.x &&
					i.x + 16 < c.x + 49 &&
					i.y + 12 > c.y &&
					i.y + 12 < c.y + 30;
}

/****************** Check if car collides with car ********************/
int GAME_NewPosCollides(short x, short y, struct car c){
	return 	(
						(y >= c.y && y <= c.y + 30) || (y + 30 >= c.y && y + 30 <= c.y + 30)
					) &&
					(
						(x >= c.x && x <= c.x + 50) || (x + 50 >= c.x && x + 50 <= c.y + 50)
					);
}

/****************** Update score on item collision ********************/
void GAME_UpdateScore(struct item* i, int carNo){
		i->active = 0;
		GLCD_Bmp (  i->x,   i->y,  23,  19, DELETEITEM);
		switch(carNo){
			case 1:
				score[player-1] += i->score * 100;
				if(score[player-1] < 0) score[player-1] = 0;
				GLCD_Bmp ( player == 1? 298 : 280,  80,  16,  20, i->SCORETEXTURE);
				break;
			case 2:
				score[2-player] += i->score * 100;
				if(score[2-player] < 0) score[2-player] = 0;
				GLCD_Bmp ( player == 2? 298 : 280,  80,  16,  20, i->SCORETEXTURE);
				break;
			default:
				break;
		}				
}

/****************** Set attributes of new generated item **************/
void GAME_ItemSetAttributes(struct item* i, int type){
	switch(type){
		case POINTP1:
			i->score = 1;
			i->TEXTURE = POINT;
			i->SCORETEXTURE = PLUSONE;
			break;
		case POINTM1:
			i->score = -1;
			i->TEXTURE = NPOINT;
			i->SCORETEXTURE = MINONE;
			break;
		case POINTRM2:
			i->score = -2;
			i->TEXTURE = RANDPOINT;
			i->SCORETEXTURE = MINTWO;
			break;
		case POINTRP2:
			i->score = +2;
			i->TEXTURE = RANDPOINT;
			i->SCORETEXTURE = PLUSTWO;
			break;
	}
}

/****************** Get item type from item score *********************/
enum ITEM_TYPE GAME_TypeFromScore(int score){
	enum ITEM_TYPE t;
	switch(score){
		case -2:
			t = POINTRM2;
			break;
		case -1:
			t = POINTM1;
			break;
		case +1:
			t = POINTP1;
			break;
		case +2:
			t = POINTRP2;
			break;
	}
	return t;
}


/*----------------------------------------------------------------------------
																	Main Program
 *----------------------------------------------------------------------------*/
int main (void) {                       /* Main Program                       */

	
  int num     = -1; 
  int dir     =  1;
  int pic     =  0;
	int bgpic = 0;
	int i, j, k;
	int kkm;
	int item_type;
	
	struct car CAR0;
	struct car CAR2;
	
	
	CAN_Init();
	CAN_TxMsg[1].id = 33;                           /* initialise message to send */
  for (i = 0; i < 8; i++) CAN_TxMsg[0].data[i] = 0;
  CAN_TxMsg[1].len = 8;
  CAN_TxMsg[1].format = STANDARD_FORMAT;
  CAN_TxMsg[1].type = DATA_FRAME;
	
	players[0] = 1;
	players[1] = 1;
	
	CAR0.x = x;
	CAR0.y = y;
	
	CAR0.TEXTURE = CAR_TEXTURE_RED;
	CAR2.TEXTURE = CAR_TEXTURE_YELLOW;
	
  LED_Init ();
  GLCD_Init();
	
  SysTick_Config(SystemCoreClock/100);  /* Generate interrupt every 10 ms     */
	GAME_InitialState();
	
	/****************************************** GAME LOOP ***************************************/			
  for (;;) {                            /* Loop forever                       */
		switch(state){
			/*********************************************************************************************/
			/*******************************************MENU STATE ***************************************/		
			/*********************************************************************************************/
			case MENU:
				GLCD_Bmp(  200,	   0,  99,  240, LOGO);
				if (ClockANI && animActive) {
					
					GLCD_Bmp (  45,   70,  25,  100, &BTN_START[pic/2*100*25]);
					if (pic++ > 14) {
						GAME_WaitingState();
						pic = 0;
					}
				}
				break;
				
				
			/*********************************************************************************************/
			/*******************************************WAIT STATE ***************************************/		
			/*********************************************************************************************/
			case WAITING:
				if (ClockANI) {
					//3 2 1 countdown
					GLCD_Bmp (  200,   108,  36,  24, &NUMBERS[pic/2000*36*24]);
					if (pic++ > 7000) {
						GAME_RunningState();
						pic = 0;
					}
				}
				break;
				
			/*********************************************************************************************/
			/*******************************************RUN STATE ****************************************/		
			/*********************************************************************************************/
			case RUN:
				
				/************************** INITIAL SYNCRONIZATION ************************************/
				if(ready == 0){
					if (CAN_TxRdy[0]) {                           /* tx message on CAN Controller #2 */
						CAN_TxRdy[0] = 0;

						CAN_TxMsg[1].data[0] = 1;             /* data[0] field = ADC value */
						CAN_wrMsg (1, &CAN_TxMsg[1]);               /* transmit message */
					}

					
					GLCD_Bmp (  200,	   76,  21,  87, WAIT);
					
					player = 1;
					while (!CAN_RxRdy[1]){
						player = 2;
					};
					if (CAN_RxRdy[1]) {                           /* rc message on CAN Controller #1 */
						CAN_RxRdy[1] = 0;

						ready = 1;
						CAN_wrMsg (1, &CAN_TxMsg[1]);               /* transmit message */
					}
					CAN_InitialMessage(player);
					
					ready = 1;
					if(player == 1){
						y = 100;
					}
					else if (player == 2){
						y = 50;
					}
					GLCD_Clear  (BGCOLOR);
					GLCD_Bmp (  0,   210,  270,  30, TRACK);
				}
				
				
				/************************** INCREASE DISTANCE *****************************************/
				km ++;
				
				
				/************************** GAME OVER *************************************************/
				if(km == TRACKLEN){
					GAME_GameOverStateFirst();
					
					j = 22*log_10(score[player-1]/100);
					
					k = 120 - 22 - j/2;
					kkm = score[player-1]/100;
					if ( kkm == 0){
						j+=11;
						GLCD_Bmp ( 200 ,	k+j,  33,  21, &NUMBIG[(9-kkm%10)*33*21]);
					}
					while (kkm != 0){
						GLCD_Bmp ( 200 ,	k+j,  33,  21, &NUMBIG[(9-kkm%10)*33*21]);
						kkm /= 10;
						j-=22;
					}
					GAME_GameOverStateSecond();
					continue;
				}
				
				
				/************************** DRAW DISTANCE *********************************************/
				i = 0;
				kkm = km/100;
				while (kkm > 0){
					GLCD_Bmp ( 300,	   230-i,  10,  6, &NUMS[(10-kkm%10)*10*6]);
					kkm /= 10;
					i+=7;
				}
				
				
				/************************** DRAW SCORES ***********************************************/
				for(i = 0; i < 4; i++){
					if(players[i] == 1){
						k = 7 * log_10(prevscore[i]/100);
						GLCD_Bmp (  300 - i*18,   30,  14,  14, &PLAYERS[((3-i)*14*14)]);
						j = 7 * log_10(score[i]/100);
						
						kkm = score[i]/100;
						if (k != j){
							GLCD_Bmp ( 300 - i*18,	50+k,  10,  6, &NUMS[0]);
						}
						if ( kkm == 0){
							j+=7;
							GLCD_Bmp ( 300 - i*18,	50+j,  10,  6, &NUMS[(10-kkm%10)*10*6]);
						}
						while (kkm != 0){
							GLCD_Bmp ( 300 - i*18,	50+j,  10,  6, &NUMS[(10-kkm%10)*10*6]);
							kkm /= 10;
							j-=7;
						}
					}
				}
				
				
				/************************** UPDATE CAR POSITION ***************************************/
				if( GAME_NewPosCollides(x, y, CAR2)){
					x = CAR0.x;
					y = CAR0.y;
				}
				else{
					CAR0.x = x;
					CAR0.y = y;
				}
				
				/************************** SAVE PREV SCORE FOR SCREEN UPDATE *************************/
				for(i = 0; i < 4; i++){
					prevscore[i] = score[i];
				}
				
				/************************** UPDATE ITEMS & SCORES *************************************/
				for(i=0; i<MAXITEMS; i++){
					if (ITEMS_ON_SCREEN[i].active == 1 && ITEMS_ON_SCREEN[i].to_signal == 0){
						ITEMS_ON_SCREEN[i].x--;
						
						if(
							GAME_ItemCollides(ITEMS_ON_SCREEN[i], CAR0)
						){
							GAME_UpdateScore(&ITEMS_ON_SCREEN[i], 1);
							
						}
						else if (
							GAME_ItemCollides(ITEMS_ON_SCREEN[i], CAR2)
						){
							GAME_UpdateScore(&ITEMS_ON_SCREEN[i], 2);							
						}
							
						if (ITEMS_ON_SCREEN[i].x < 20){
							ITEMS_ON_SCREEN[i].active = 0;
							GLCD_Bmp (  ITEMS_ON_SCREEN[i].x,   ITEMS_ON_SCREEN[i].y,  23,  19, DELETEITEM);
						}
					}
				}
				
				
				/************************** GENERATE ITEMS (PL1) **************************************/
				if(player == 1){
					for(i=0; i<MAXITEMS; i++){
						if (ITEMS_ON_SCREEN[i].active == 0){
							if(rand() % 500 == 0){
								ITEMS_ON_SCREEN[i].active = 1;
								ITEMS_ON_SCREEN[i].x = 250;
								ITEMS_ON_SCREEN[i].y = 40 + rand()%120;
								ITEMS_ON_SCREEN[i].to_signal = 1;
								item_type = rand()%4;
								GAME_ItemSetAttributes(&ITEMS_ON_SCREEN[i], item_type);
							}
							break;
						}
					}
				}
				
				/************************** DRAW ITEMS ON SCREEN **************************************/
				for(i=0; i<MAXITEMS; i++){
					if (ITEMS_ON_SCREEN[i].active == 1){
						GLCD_Bmp (  ITEMS_ON_SCREEN[i].x,   ITEMS_ON_SCREEN[i].y,  23,  19, ITEMS_ON_SCREEN[i].TEXTURE);
					}
				}
				
				
				/************************** SYNC SEND MESSAGE *****************************************/
				if (CAN_TxRdy[1]) {                           /* tx message on CAN Controller #2 */
					
					CAN_TxRdy[1] = 0;

					CAN_TxMsg[1].data[0] = CAR0.x;             /* data[0] field = ADC value */
					CAN_TxMsg[1].data[1] = CAR0.y;
					CAN_TxMsg[1].data[2] = (char)(km/100);
					CAN_TxMsg[1].data[3] = (char)(km%100);
					CAN_TxMsg[1].data[4] = (char)(score[player-1]/100);
					CAN_TxMsg[1].data[5] = (char)(score[player-1]%100);
					CAN_TxMsg[1].data[6] = 0;
					CAN_TxMsg[1].data[7] = 0;
					if(player == 1){
						for(i=0; i<MAXITEMS; i++){
							if (ITEMS_ON_SCREEN[i].to_signal == 1){
								ITEMS_ON_SCREEN[i].to_signal = 0;
								CAN_TxMsg[1].data[6] = ITEMS_ON_SCREEN[i].y;
								CAN_TxMsg[1].data[7] = 2 + ITEMS_ON_SCREEN[i].score;
								break;
							}
						}
					}
					
					CAN_wrMsg (2, &CAN_TxMsg[1]);               /* transmit message */
				}
	
				/************************** SYNC RECV MESSAGE *****************************************/
				if (CAN_RxRdy[0]) {                           /* rc message on CAN Controller #1 */
					
					CAR2.x = 							CAN_RxMsg[0].data[0];
					CAR2.y = 							CAN_RxMsg[0].data[1];
					km = 									CAN_RxMsg[0].data[2]*100;
					km += 								CAN_RxMsg[0].data[3];
					score[2-player] = 		CAN_RxMsg[0].data[4]*100;
					score[2-player] += 		CAN_RxMsg[0].data[5];
					if(CAN_RxMsg[0].data[6] != 0 && player != 1){
						for(i=0; i<MAXITEMS; i++){
							if (ITEMS_ON_SCREEN[i].active == 0){
								ITEMS_ON_SCREEN[i].to_signal = 0;
								ITEMS_ON_SCREEN[i].x = 250;
								ITEMS_ON_SCREEN[i].y = CAN_RxMsg[0].data[6];
								ITEMS_ON_SCREEN[i].active = 1;
								ITEMS_ON_SCREEN[i].score = CAN_RxMsg[0].data[7] - 2;
								GAME_ItemSetAttributes(&ITEMS_ON_SCREEN[i], GAME_TypeFromScore(ITEMS_ON_SCREEN[i].score));
								break;
							}
						}
					}
					CAN_RxRdy[0] = 0;
				}
				
				
				/************************** DRAW CARS ON SCREEN ***************************************/
				if(player == 1){
					GLCD_Bmp (  CAR0.x,   CAR0.y,  49,  30, CAR0.TEXTURE);
					GLCD_Bmp (  CAR2.x,   CAR2.y,  49,  30, CAR2.TEXTURE);
				}
				else if (player == 2){
					GLCD_Bmp (  CAR0.x,   CAR0.y,  49,  30, CAR2.TEXTURE);
					GLCD_Bmp (  CAR2.x,   CAR2.y,  49,  30, CAR0.TEXTURE);
				}
				
				/************************** DRAW CAR ON RIGHT SIDE ************************************/
				GLCD_Bmp (14 + km*228/TRACKLEN, 221, 14, 8, MINICAR);
				
				
				/************************** ANIMATE BACKGROUND ****************************************/
				if (ClockANI){
					GLCD_Bmp (  0,   0,  320,  27, &BGBARS[(bgpic)*320*27]);
					GLCD_Bmp (  0,   183,  320,  27, &BGBARS[(bgpic)*320*27]);
					if (bgpic++ > 1)  bgpic = 0;
					ClockANI = 0;
				}
				break;

			/*********************************************************************************************/
			/*******************************************END STATE ****************************************/		
			/*********************************************************************************************/
			case END:
				if(restart == 1){
					km = 0;
					for (i = 0; i < 4; i++){
						score[i] = 0;
					}
					
					GAME_InitialState();
					CAN_InitialMessage(player);
				}
				break;
			}
		
    

    if (ClockLEDOn) {    /* Blink LED every 1 second (for 0.5s)*/
      ClockLEDOn  = 0;

      /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...           */
      num += dir;
      if (num == LED_NUM) { dir = -1; num =  LED_NUM-1; } 
      else if   (num < 0) { dir =  1; num =  0;         }
    
      LED_On (num);
    }
    if (ClockLEDOff) {
      ClockLEDOff = 0;

      LED_Off(num);
    }
		
  }
}


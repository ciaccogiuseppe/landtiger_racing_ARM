/*----------------------------------------------------------------------------
 * Name:    CAN.h
 * Purpose: low level CAN definitions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*
 * Modified by:			Giuseppe Ciacco
 * Modified date:		03/04/2022
 * Descriptions:		adapting functions to LPC1768 + higher level communication functions
 *----------------------------------------------------------------------------*/

#ifndef __CAN_H
#define __CAN_H

#include <stdint.h>

#define STANDARD_FORMAT  0
#define EXTENDED_FORMAT  1

#define DATA_FRAME       0
#define REMOTE_FRAME     1

#define SND 0

typedef struct  {
  unsigned int   id;                    /* 29 bit identifier */
  unsigned char  data[8];               /* Data field */
  unsigned char  len;                   /* Length of data field in bytes */
  unsigned char  format;                /* 0 - STANDARD, 1- EXTENDED IDENTIFIER */
  unsigned char  type;                  /* 0 - DATA FRAME, 1 - REMOTE FRAME */
} CAN_msg;

/* Functions defined in module CAN.c */
void CAN_setup         (uint32_t ctrl);
void CAN_start         (uint32_t ctrl);
void CAN_waitReady     (uint32_t ctrl);
void CAN_wrMsg         (uint32_t ctrl, CAN_msg *msg);
void CAN_rdMsg         (uint32_t ctrl, CAN_msg *msg);
void CAN_wrFilter      (uint32_t ctrl, uint32_t id, uint8_t filter_type);
void CAN_transmitBuffer (uint32_t ctrl, void* buf, uint32_t len);
void CAN_receiveBuffer (uint32_t ctrl, void* buf, uint32_t len);
void CAN_transmitBuffers (uint32_t ctrl, void* buf, uint32_t len);
void CAN_receiveBuffers (uint32_t ctrl, void* buf, uint32_t len);

extern CAN_msg       CAN_TxMsg[2];      /* CAN messge for sending */
extern CAN_msg       CAN_RxMsg[2];      /* CAN message for receiving */                                
extern unsigned int  CAN_TxRdy[2];      /* CAN HW ready to transmit a message */
extern unsigned int  CAN_RxRdy[2];      /* CAN HW received a message */

#endif



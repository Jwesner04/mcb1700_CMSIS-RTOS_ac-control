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

#include "LPC17xx.H"                         /* LPC17xx definitions           */
#include "LED.h"
#include "ADC.h"
#include "mcb1700_joystick.h"

uint8_t  clock_1s;                           /* Flag activated each second    */
uint8_t LED_pos = 4;

/*----------------------------------------------------------------------------
  Systick Interrupt Handler
  SysTick interrupt happens every 10 ms
 *----------------------------------------------------------------------------*/
//void SysTick_Handler (void) {
//	
//	#define JOY_POS_UP     (1<<3)
//  #define JOY_POS_DOWN	 (1<<5)
//  static unsigned long ticks = 0;
//	
//	static unsigned int  leds = 0x00;
//	static unsigned  LEDIND = 1;
//	
//	
////if (ticks++ >= 49) {                     
////    ticks    = 0;
////    clock_1s++;
////		if (LEDIND == 1)
////		{
////			LED_Off(0);
////			LEDIND=0;
////		
////		}
////		else 
////		{
////			LED_On(0);
////			LEDIND=1;
////		}
////		
////	}

////	if(JoyPosGet() == JOY_POS_RIGHT)
////	{
////		if(LED_pos < 7)
////		{
////			if(clock_1s > 0){ 
////				clock_1s = 0;
////				LED_Off(LED_pos);
////				LED_pos++;
////				LED_On(LED_pos);
////			}
////		}

////		
////	}
////	
////	if(JoyPosGet() == JOY_POS_LEFT)
////	{
////		if(LED_pos > 1)
////		{
////			if(clock_1s > 0){
////				clock_1s = 0;
////				LED_Off(LED_pos);
////				LED_pos--;
////				LED_On(LED_pos);
////			}
////		}
////	}

//}

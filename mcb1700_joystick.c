/* 
Name:            mcb1700_joystick.c
Programmer:      Jacob Violet
Project:         Lab 2
Date Made:       8/31/2016
Last Modified:   9/2/2016
Class:           ECE 485
Groupt Members:  Jacob Violet, Jon Wesner

Description: This file contains code return joystick position for LPC 1768

------------------------------------------------------------------------------*/



/*
*********************************************************************************************************
*                                         BSP_JoyPosGet()
*
* Description : Return the joystick position. Joystick is active low
* Argument(s) : none
*
* Return(s)   : The joystick postition.
*
*                        JOY_POS_LEFT    If the joystick is toggled left.
*                        JOY_POS_RIGHT   If the joystick is toggled right.
*                        JOY_POS_UP      If the joystick is toggled up.
*                        JOY_POS_DOWN    If the joystick is toggled down.
*                        JOY_POS_CENTER  If the joystick is being pressed
*                        JOY_POS_NONE    If the joystick is not being pressed.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
 
 #include "LPC17xx.H"                         /* LPC17xx definitions           */
 #include "mcb1700_joystick.h" 
 
uint8_t JoyPosGet (void) 
{
	
		#define JOY_POS_CENTER (1<<0)
    #define JOY_POS_UP     (1<<3)
    #define JOY_POS_RIGHT	 (1<<4)
    #define JOY_POS_DOWN	 (1<<5)
    #define JOY_POS_LEFT   (1<<6)
	  #define JOY_POS_NONE   (1<<7)
		#define KBD_MASK 0x79
		uint8_t kbd_val;
		uint8_t val;
	
	  LPC_PINCON->PINSEL3 &= ~((3<< 8)|(3<<14)|(3<<16)|(3<<18)|(3<<20)); 
		LPC_GPIO1->FIODIR &= ~((1<<20)|(1<<23)|(1<<24)|(1<<25)|(1<<26));
    kbd_val = (LPC_GPIO1->FIOPIN >> 20) & KBD_MASK;
		val = (~kbd_val & KBD_MASK);
	          
	
		/* Joystick center was pressed */
		if( val == JOY_POS_CENTER )
	  {
	   	 return JOY_POS_CENTER;
	  }
	  /* Joystick down was pressed */
	  else if( val == JOY_POS_DOWN )
	  {
	   	 return JOY_POS_DOWN;
	  }
	  /* Joystick left was pressed */
	  else if( val == JOY_POS_LEFT  )
	  {
	   	 return JOY_POS_LEFT;
	  } 
	  /* Joystick right was pressed */
	  else if( val == JOY_POS_RIGHT	 )
	  {
	   	 return JOY_POS_RIGHT	;
	  } 
	  /* Joystick up was pressed */
	  else if( val == JOY_POS_UP ) 
	  {
	   	 return JOY_POS_UP;
	  }
		/* No joystick input */
		else
		{
			 return JOY_POS_NONE;
		}

 }
    

/*----------------------------------------------------------------------------
 * Name:    Jacob Violet & Jon Wesner
 * Purpose: Tic-Tac-Toe Game
 * Note(s): Joystick is used to control game
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "LPC17xx.H"                         /* LPC17xx definitions           */
#include "GLCD.h"
#include "Serial.h"
#include "LED.h"
#include "ADC.h"
#include "mcb1700_joystick.h"
#include <stdbool.h>
#include "cmsis_os.h"
#include "LPC17xx.h"

#define __FI        1                       /* Font index 16x24               */
                                                                         
char text[10]; 								//used to format the output
uint32_t threshTemp = 60;  	 //holds the setup temp
uint32_t roomTemp = 60;				//holds current room temp
uint32_t ADC_value;				//holds the value from the potentiometer
bool fanPower = false;		//controls state of fan power
bool acPower = false;			//controls state of ac power

/*----------------------------------------------------------------------------
 *      Thread Declaration
 *---------------------------------------------------------------------------*/
osThreadId tid_LED_UPDATE;                  /* Thread id of task: phase_a */
osThreadId tid_JOYSTICK;                  /* Thread id of task: phase_b */
osThreadId tid_displayToLCD;  


/*----------------------------------------------------------------------------
 *      Semaphore Declaration
 *---------------------------------------------------------------------------*/
osSemaphoreId displayReady;
osSemaphoreDef(displayReady);
osSemaphoreId debouncer;                         // Semaphore ID
osSemaphoreDef(debouncer);                       // Semaphore definition


/*----------------------------------------------------------------------------
 *      Function Hex to BCD converter
 *---------------------------------------------------------------------------*/
unsigned char hex2bcd (unsigned char x)
{
    unsigned char y;
    y = (x / 10) << 4;
    y = y | (x % 10);
    return (y);
}


/*----------------------------------------------------------------------------
 *      Thread 1 LED_UPDATE
 *---------------------------------------------------------------------------*/
void LED_UPDATE (void const *argument) {
  for (;;) {
    LED_On(0);							// turn led on
		osDelay(1000);         // delay 1000ms                   
    LED_Off(0);						// turn led off
		osDelay(1000);        // delay 1000ms
	}
}

/*----------------------------------------------------------------------------
 *      Thread 2 Joystick
 *---------------------------------------------------------------------------*/
void JOYSTICK (void const *argument) {
  #define JOY_POS_UP     (1<<3)		//setup joystick positions
  #define JOY_POS_RIGHT	 (1<<4)
  #define JOY_POS_DOWN	 (1<<5)
  #define JOY_POS_LEFT   (1<<6)
	#define JOY_POS_CENTER (1<<0)
	
  for (;;) {
		osSemaphoreWait (debouncer, osWaitForever);  //wait for display to update
		
			// UP increases temperature by 1 degree
			if(JoyPosGet() == JOY_POS_UP){
				threshTemp = threshTemp + 1;
			}
			
			// Down decrease temp. by 1 degree
			if(JoyPosGet() == JOY_POS_DOWN){
				threshTemp = threshTemp - 1;
			}

			// Left Fan manual = True or auto = False
			if(JoyPosGet() == JOY_POS_LEFT){
				if(fanPower){
					fanPower = false;
				}
				else{
					fanPower = true;
				}
			}
		
			// Right Turn AC On/Off
			if(JoyPosGet() == JOY_POS_RIGHT){
				if(acPower){
					acPower = false;
				}
				else{
					acPower = true;
				}
			}
			
			// Read potentiometer
			ADC_StartCnv();
			ADC_value = ADC_GetCnv()+1500;
			ADC_StopCnv();
			
			osSemaphoreRelease (displayReady); //trigger update display thread
	}
}

/*----------------------------------------------------------------------------
 *      Thread 3  displayToLCD
 *---------------------------------------------------------------------------*/
void displayToLCD (void  const *argument) {
	
				//local variables
	uint8_t currentTempDisplay[] = "0000000000";    
	uint8_t setupTempDisplay[] = "0000000000";
	bool acState = false;
	bool fanState = false;
	static uint8_t On[] = "ON  "; 		//text for printing
	static uint8_t Off[] = "OFF ";
	static uint8_t Auto[] = "AUTO  ";
	static uint8_t Manual[] = "Manual";
	
	for(;;){
		osSemaphoreWait (displayReady, osWaitForever);  //wait for joystick release
		
		// computation section, manage the states
		if(acPower){
			if((ADC_value >>6) > threshTemp){  //temp controls on/off ac
				acState = true;
			}
			else{
				acState = false;
			}
		}
		else {
			acState = false;
		}
		

		
		// Display info to LCD below
		
		sprintf(text, "%02X", hex2bcd(threshTemp));   //format setup temp
		GLCD_DisplayString(6,  11, __FI,  (unsigned char *)text); //print setup temp
		
		sprintf(text, "%02X", hex2bcd(ADC_value >>6)); //format actual temp
		GLCD_DisplayString (5, 13, __FI, text);     //print actual temp
	
		
		if(acState){
			GLCD_DisplayString (7, 11, __FI, On);  //print ac status on
			
			// LED management below
			LED_On(3);
		}
		else{
			GLCD_DisplayString (7, 11, __FI, Off);  //print ac status off
			
			// LED management below
			LED_Off(3);
		}
		
		
		if(fanPower){
			GLCD_DisplayString (8, 11, __FI, Manual);   //print fan status manual
			
			// LED management below
			LED_On(4);
		}
		else{
			GLCD_DisplayString (8, 11, __FI, Auto); //print fan status auto
			
			// LED management below
			if(!fanState){
				LED_Off(4);
			}
		}
		
		
		osDelay(1500); // Debouncer 
		osSemaphoreRelease (debouncer);   //triggers joystick thread
	}
}


/*----------------------------------------------------------------------------
 *      Thread Definitions
 *---------------------------------------------------------------------------*/

osThreadDef(displayToLCD,  osPriorityNormal, 1, 0);
osThreadDef(LED_UPDATE, osPriorityNormal, 1, 0);
osThreadDef(JOYSTICK, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *      Main: Initialize LCD,Led,Ser,ADC
 *---------------------------------------------------------------------------*/
int main (void) {
	
/*----------------------------------------------------------------------------
 *      Initialization of LCD/LED/SER/ADC
 *---------------------------------------------------------------------------*/
	#ifdef __USE_LCD
  GLCD_Init();                               
	LED_Init();			
	SER_Init();
	ADC_Init();

  GLCD_Clear(White);                         
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(White);
  GLCD_DisplayString(0, 0, __FI, "       TIC-TAC-TOE      ");
	
	// Display Tic Tac Toe Board
	GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
	GLCD_DisplayString (2, 4, __FI, "|"); 
	GLCD_DisplayString (3, 4, __FI, "|"); 
	GLCD_DisplayString (5, 4, __FI, "|"); 
	GLCD_DisplayString (7, 4, __FI, "|"); 
	GLCD_DisplayString (8, 4, __FI, "|"); 
	GLCD_DisplayString (4, 0, __FI, " -----------");
	GLCD_DisplayString (6, 0, __FI, " -----------"); 
	GLCD_DisplayString (2, 8, __FI, "|"); 
	GLCD_DisplayString (3, 8, __FI, "|"); 
	GLCD_DisplayString (5, 8, __FI, "|"); 
	GLCD_DisplayString (7, 8, __FI, "|"); 
	GLCD_DisplayString (8, 8, __FI, "|"); 
#endif

/*----------------------------------------------------------------------------
 *      Thread creation
 *---------------------------------------------------------------------------*/
  tid_LED_UPDATE = osThreadCreate(osThread(LED_UPDATE), NULL);
  tid_JOYSTICK = osThreadCreate(osThread(JOYSTICK), NULL);
	tid_displayToLCD = osThreadCreate(osThread(displayToLCD),  NULL);

/*----------------------------------------------------------------------------
 *    	Semaphore Creation
 *---------------------------------------------------------------------------*/
	displayReady = osSemaphoreCreate(osSemaphore(displayReady), 0);                        
	debouncer = osSemaphoreCreate(osSemaphore(debouncer), 0);
	
/*----------------------------------------------------------------------------
 *      Enable tasks to run
 *---------------------------------------------------------------------------*/
	osSemaphoreRelease (debouncer);
	
  osDelay(osWaitForever);
  while(1);
}


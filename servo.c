/**********************************************************
* servo control
* servo should be connected to OC0 (portb3)
*
* @Author   : Simon Schulz [avr<AT>auctionant.de]
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This program is distributed in the hope that it will be useful, but
*
* WITHOUT ANY WARRANTY;
*
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc., 51
* Franklin St, Fifth Floor, Boston, MA 02110, USA
*
* http://www.gnu.de/gpl-ger.html
*
* ----------------------------------------------------------
* (Ingo Busker http://www.mikrocontroller.com):
* - using OC2 (PD.7) instead of of OC0 (PB.3), if ISA_CTRL is defined (in main.h)
* - clock div is 1024 
*   (for use with ISA-CTRL - pcb)  
*   removed (double) calculation for pwm, this saves some memory
*
*  
***********************************************************/
// #include "main.h"
#include "servo.h"

#include <avr/pgmspace.h>
#include "uip.h"
#include "nic.h"
#include "uip_arp.h"
#include "compiler.h"
#include "services.h"
#include "usart.h"
#include "mca25.h"
#include "servo.h" 

#define SYSCLK F_CPU

#if USE_SERVO
volatile unsigned char servo_pos;
volatile unsigned char servo_need_update=0;

void servo_init (void){
	//set up fast pwm mode:
#if defined (__AVR_ATmega644__)
	TCCR2A = 1<<WGM00|1<<WGM01|1<<COM2A1; 
	TCCR2B = 1<<CS00|1<<CS01|1<<CS02;
#endif	

#if defined (__AVR_ATmega32__)
	TCCR2 = 1<<WGM00|1<<WGM01|1<<COM01|1<<CS00|1<<CS01|1<<CS02;
#endif
//	#define WGM_CFG (1<<WGM00 | 1<<WGM01) //fast pwm
//	#define COM_CFG (1<<COM0A1 | 0<<COM0A0) //clr on match, set on max
//	#define CLK_CFG (1<<CS00 | 1<<CS01 | 1<<CS02) //set up clock source clk/1024
//	TCCR2A = WGM_CFG | COM_CFG ; 
//	TCCR2B = CLK_CFG;
	DDRD |= 0x80;
//	servo_set_pos(127);
servo_set_pos(127);
}

void servo_prepare_pos(unsigned char val){
	servo_pos = val;
	servo_need_update = 1;
}

void servo_set_pos(unsigned char val){
	servo_pos = val;
	servo_move();
}

void servo_move(void){
	//we have an overflow every 8.8ms
	//so we set out compare value between 
	//28 (=1ms) and 56 (=2ms)
	//this is a poor res (only 28values)
	//and the interval of 8.8ms is outsid
	//the servo spec (every 20ms) but it works 
	//for most servos
	//val = 0   -> 1ms on, 7.8ms off
	//val = 255 -> 2ms on, 6.8ms off
    #define CALC_1MS (SYSCLK/1024/1000) //=14.4 at 14.7456MHz 
	// res is only 14, but it works...
#if defined (__AVR_ATmega644__)
	OCR2A = CALC_1MS-1 + (unsigned char)(((unsigned int)servo_pos*CALC_1MS)/255);
#endif	

#if defined (__AVR_ATmega32__)
	OCR2 = ((F_CPU/1024/1000)-1) + (unsigned char)((unsigned int)servo_pos*(F_CPU/1024/1000)/255);
#endif

//    #define CALC_1MS SYSCLK/256/1000
//    OCR0 = (CALC_1MS-5 + (unsigned char)(((double)servo_pos/255.0)*CALC_1MS));
	servo_need_update = 0;
}

#endif 


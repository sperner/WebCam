/*****************************************************************************
*  Module Name:       uIP-AVR Port - main control loop shell
*  
*  Created By:        Louis Beaudoin (www.embedded-creations.com)
*
*  Original Release:  September 21, 2002 
*
*  Modifications from Sven Sperner <cethss@gmail.com>
*
*  Module Description:  
*  This main control loop shell provides everything required for a basic uIP
*  application using the RTL8019AS NIC
*
*****************************************************************************/


#include <avr/pgmspace.h>
#include "uip.h"
#include "nic.h"
#include "uip_arp.h"
#include "compiler.h"
#include "services.h"
#include "usart.h"
#include "mca25.h"
#include "servo.h" 
//#include "clock.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
//#define LED_RED (PIND & (1 <<7))



volatile unsigned char usart_timeout;
mca25_t mca25_stat;


/*****************************************************************************
*  Periodic Timout Functions and variables
*
*  The periodic timeout rate can be changed depeding on your application
*  Modify these functions and variables based on your AVR device and clock
*    rate
*  The current setup will interrupt every 256 timer ticks when the timer
*    counter overflows.  timerCounter must count until 0.5 seconds have
*    alapsed
*****************************************************************************/

#define TIMER_PRESCALE    1024
#define TIMERCOUNTER_PERIODIC_TIMEOUT (F_CPU / TIMER_PRESCALE / 2 / 256)

static unsigned char timerCounter;

void initTimer(void)
{
  TCCR0=5; //outp( 5, TCCR0 ) ;  // timer0 prescale 1/1024

  // interrupt on overflow
  TIMSK|=(1<<TOIE0); //sbi( TIMSK, TOIE0 ) ;
  timerCounter = 0;
}



#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OVERFLOW0:iv_TIMER0_OVF
#endif

SIGNAL(SIG_OVERFLOW0)
{
  timerCounter++;
  if((unsigned char)!(timerCounter&3)) if (usart_timeout) usart_timeout--;
}



/*****************************************************************************
*  Main Control Loop
*
*  
*****************************************************************************/
int main(void)
{
 	//led PB4
	// Pins als Ausgänge definieren:
	DDRB  |= (1 << 4);

    PORTB |= (0 << 4);

//DDRD |= (1 << 7);
//PORTD |= (1 << 7);   
  
  unsigned char i;
  unsigned char arptimer=0;

   // init NIC device driver
  nic_init();

  // init uIP
  uip_init();

  // init app
  services_init();

  // init ARP cache
  uip_arp_init();

  // init periodic timer
  initTimer();

  SET_USART_9600();
  //PORTB |= (1 << 4); //LED PB4 ein
  sei();

//  if((mca25_stat.init=mca25_init())==0) mca25_stat.init=mca25_configure();
//  if(mca25_stat.init) {

    // print error message?;
//  }
#if USE_MCA25_CAM
//DEBUG:
	MCA25_ERROR_LED_OFF();
	MCA25_CLOCK_LED_OFF();
	//DDRB = 0xFF;
	
	MCA25_STATUS_LED_ON();
	mca25_init();
	mca25_configure();	
	MCA25_STATUS_LED_OFF();
	MCA25_ERROR_LED_ON();
	MCA25_CLOCK_LED_ON();
#endif	
//	#if USE_CLOCK
//		Start_Clock();
	//#endif  
#if USE_SERVO
	servo_init();
#endif	
//PORTD |= (1 << 7);
  while(1)
  {
    // look for a packet
    uip_len = nic_poll();
    if(uip_len == 0)
    {
      // if timed out, call periodic function for each connection
      if(timerCounter > TIMERCOUNTER_PERIODIC_TIMEOUT)
      {
        timerCounter = 0;
        
        for(i = 0; i < UIP_CONNS; i++)
        {
          uip_periodic(i);
		
          // transmit a packet, if one is ready
          if(uip_len > 0)
          {
            uip_arp_out();
            nic_send();
          }
        }

        /* Call the ARP timer function every 10 seconds. */
        if(++arptimer == 20)
        {	
          uip_arp_timer();
          arptimer = 0;
        }
      }
    }
    else  // packet received
    {
      // process an IP packet
      if(BUF->type == htons(UIP_ETHTYPE_IP))
      {
        // add the source to the ARP cache
        // also correctly set the ethernet packet length before processing
        uip_arp_ipin();
        uip_input();

        // transmit a packet, if one is ready
        if(uip_len > 0)
        {
          uip_arp_out();
          nic_send();
        }
      }
      // process an ARP packet
      else if(BUF->type == htons(UIP_ETHTYPE_ARP))
      {
        uip_arp_arpin();

        // transmit a packet, if one is ready
        if(uip_len > 0)
          nic_send();
      }
    }
  }

  return 1;
}

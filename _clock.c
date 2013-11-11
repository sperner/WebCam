/*#######################################################################################
AVR Small Webserver 

Copyright (C) 2004 Ulrich Radig

#######################################################################################*/

//#include "main.h"
#include "clock.h"

unsigned long ActualTime = 0;
unsigned int Count_ms = 0, Count_min = 0, Count_sec = 0 ;

#if USE_CLOCK

//############################################################################
//Diese Routine addiert 1Sekunde wenn Timer Interrupt ausgelöst wird 
SIGNAL (SIG_OVERFLOW1)
//############################################################################
{
    static unsigned int t=0;
	static unsigned int t2=0;
	
	TCNT1 = 65535 - (SYSCLK / 1000);
	#if USE_MCA25_CAM
	if ((Count_sec & 0x01)==1){
		MCA25_CLOCK_LED_ON();
	}else{
		MCA25_CLOCK_LED_OFF();
	}
#endif	

	Count_ms++;
	t++;
	if ((t % 1000) == 0) {  Count_sec++; ActualTime++;  } // add 1 second
	if ((t % 60000) == 0) {  t=0; Count_min++; }; 
	
	
}


//############################################################################
//Diese Routine startet und inizialisiert den Timer
void Start_Clock (void)
//############################################################################
{
	TCNT1 = 65535 - (SYSCLK / 1000);
	//Interrupt for the Clock enable
	//Setzen des Prescaler auf 1024 
//	TCCR1B |= (1<<CS10 | 0<<CS11 | 1<<CS12); 
	TCCR1B |= (1<<CS10) ; 
//	timer_enable_int(_BV(TOIE1));                // not working with MEGA644
    TIMSK1 |= _BV(TOIE1);
	return;
}

//-------------------------------------------------
unsigned int SetDelay_ms (unsigned int t)
{
  return(Count_ms + t - 1);                                             
}

//-------------------------------------------------
char CheckDelay_ms (unsigned int t)
{
  return(((t - Count_ms) & 0x8000) >> 8);
}

//-------------------------------------------------
unsigned int SetDelay_sec (unsigned int t)
{
  return(Count_sec + t - 1);                                             
}

//-------------------------------------------------
char CheckDelay_sec (unsigned int t)
{
  return(((t - Count_sec) & 0x8000) >> 8);
}

//-------------------------------------------------
unsigned int SetDelay_min (unsigned int t)
{
  return(Count_min + t - 1);                                             
}

//-------------------------------------------------
char CheckDelay_min (unsigned int t)
{
  return(((t - Count_min) & 0x8000) >> 8);
}
#endif //#if USE_CLOCK


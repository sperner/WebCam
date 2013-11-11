/*#######################################################################################
AVR Small Webserver 

Copyright (C) 2004 Ulrich Radig

#######################################################################################*/

#ifndef _CLOCK_H
 #define _CLOCK_H

//Use the Clock then set USE_CLOCK 1
#define USE_CLOCK	1

extern unsigned long ActualTime;
extern unsigned int Count_ms, Count_sec, Count_min;

extern void Start_Clock (void);
extern char CheckDelay_ms (unsigned int t);
extern unsigned int SetDelay_ms (unsigned int t);
extern char CheckDelay_sec (unsigned int t);
extern unsigned int SetDelay_sec (unsigned int t);
extern char CheckDelay_min (unsigned int t);
extern unsigned int SetDelay_min (unsigned int t);


#endif //_CLOCK_H

#ifndef _APP_H_
#define _APP_H_

/*****************************************************************************
* Simple Application using uIP 0.9 
*****************************************************************************/

#include <avr/pgmspace.h>


struct smtp_state {
   unsigned char state;
   prog_char *dataptr_P;
   unsigned int dataleft;
};

struct http_state {
   unsigned char state; 
   prog_char *dataptr_P;
   void (*fp)(void);
   union {
   prog_char *nxtdatptr_P; 
   unsigned int dataleft;
   };
};

struct telnet_state {
   char flag;
};

union services_state {
   struct http_state dummy1;
   struct telnet_state dummy2;
   struct smtp_state dummy3;
};

/* UIP_APPSTATE_SIZE: The size of the application-specific state
   stored in the uip_conn structure. VERY IMPORTANT!!!! */

#define UIP_APPSTATE_SIZE sizeof(union services_state)

#include "uip.h"

void services_init(void);
void services_main(void);

#define FS_STATISTICS 0

#define UIP_APPCALL     services_main


#endif

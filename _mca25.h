#ifndef _MCA25_H_
#define _MCA25_H_

#include <avr/io.h>

#define ISA_CTRL
#define USE_MCA25_CAM 1
#define USE_WEBCAM_LEDS 1

#define CAM_BUFFER_LEN 512	
#define MCA25_COMM_BUFFER_LEN 40

#define MCA25_RESET_PORT     PORTD
#define MCA25_RESET_PORT_DIR DDRD
#define MCA25_RESET_PIN      PD6
#define MCA25_RESET_HI() MCA25_RESET_PORT|=(1<<MCA25_RESET_PIN);
#define MCA25_RESET_LO() MCA25_RESET_PORT&=~(1<<MCA25_RESET_PIN);

#if USE_WEBCAM_LEDS
#ifdef	 ISA_CTRL
	//led pin config:
	#define MCA25_STATUS_PIN     3
	#define MCA25_ERROR_PIN      5
	#define MCA25_CLOCK_PIN      4
#else
	//led pin config:
        #define MCA25_STATUS_PIN     1
        #define MCA25_ERROR_PIN      2
        #define MCA25_CLOCK_PIN      4
#endif

	//status led 
	#define MCA25_STATUS_LED_OFF() MCA25_RESET_PORT|=(1<<MCA25_STATUS_PIN);
	#define MCA25_STATUS_LED_ON()  MCA25_RESET_PORT&=~(1<<MCA25_STATUS_PIN);
	
	//error led on pin b2
	#define MCA25_ERROR_LED_OFF() MCA25_RESET_PORT|=(1<<MCA25_ERROR_PIN);
	#define MCA25_ERROR_LED_ON()  MCA25_RESET_PORT&=~(1<<MCA25_ERROR_PIN);
	
	//clock led on pin b4
	#define MCA25_CLOCK_LED_OFF() MCA25_RESET_PORT|=(1<<MCA25_CLOCK_PIN);
	#define MCA25_CLOCK_LED_ON()  MCA25_RESET_PORT&=~(1<<MCA25_CLOCK_PIN);
#else
	//DISABLE LED MACROS
	//status led
	#define MCA25_STATUS_LED_OFF() nop();
        #define MCA25_STATUS_LED_ON()  nop();

	//error led on pin b2
	#define MCA25_ERROR_LED_OFF() nop();
        #define MCA25_ERROR_LED_ON()  nop();
	
	//clock led on pin b4
	#define MCA25_CLOCK_LED_OFF() nop();
        #define MCA25_CLOCK_LED_ON()  nop();
#endif	


typedef struct {
  unsigned char       init:3;
  unsigned char       busy:1;
  unsigned char   skip_pic:1;
  unsigned char        ret:1;
  unsigned char first_data:1;
} mca25_t;

char mca25_init(void);
char mca25_configure(void);
void mca25_grab_data(char *buffer, unsigned int *datalen, char *frametype);
void mca25_grab_jpeg(char res);
void mca25_start_image_grab(void);
unsigned char mca25_copy_data(char *buffer, unsigned int *len);
void mca25_pgm_send(PGM_P pointer, unsigned int len);

#endif

/**********************************************************
* driver for MCA-25 camera
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
***********************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "delay.h"
#include "usart.h"
#include "mca25.h"


extern volatile unsigned char usart_timeout;
extern mca25_t mca25_stat;
unsigned char mca25_buf[CAM_BUFFER_LEN];


PROGMEM char MCA25_START_JPG_1[] = {
0xF9,0x81,0xEF,0x3F,0x83,0x00,0x82,0x71,0x00,0x58,0x3C,0x6D,
0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,0x6E,0x67,0x2D,0x63,0x6F,
0x6D,0x6D,0x61,0x6E,0x64,0x20,0x76,0x65,0x72,0x73,0x69,0x8C,
0xF9,0xF9,0x81,0xEF,0x3F,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,
0x22,0x20,0x74,0x61,0x6B,0x65,0x2D,0x70,0x69,0x63,0x3D,0x22,
0x4E,0x4F,0x22,0x20,0x73,0x65,0x6E,0x64,0x2D,0x70,0x69,0x78,
0x8C,0xF9,0xF9,0x81,0xEF,0x3F,0x65,0x6C,0x2D,0x73,0x69,0x7A,
0x65,0x3D,0x22};

PROGMEM char MCA25_START_JPG_640x480[] = {
0x36,0x34,0x30,0x2A,0x34,0x38,0x30};

PROGMEM char MCA25_START_JPG_320x240[] = {
0x33,0x32,0x30,0x2A,0x32,0x34,0x30};

PROGMEM char MCA25_START_JPG_160x120[] = {
0x31,0x36,0x30,0x2A,0x31,0x32,0x30};

PROGMEM char MCA25_START_JPG_2[] = {
0x22,0x20,
0x7A,0x6F,0x6F,0x6D,0x3D,0x22,0x31,0x30,0x22,0x2F,0x3E,0x42,
0x00,0x8C,0xF9,0xF9,0x81,0xEF,0x3F,0x21,0x78,0x2D,0x62,0x74,
0x2F,0x69,0x6D,0x61,0x67,0x69,0x6E,0x67,0x2D,0x6D,0x6F,0x6E,
0x69,0x74,0x6F,0x72,0x69,0x6E,0x67,0x2D,0x69,0x6D,0x61,0x67,
0x65,0x00,0x8C,0xF9,0xF9,0x81,0xEF,0x0D,0x4C,0x00,0x06,0x06,
0x01,0x80,0x4B,0xF9};


PROGMEM char MCA25_START_CAPTURING[] = {
0xF9,0x81,0xEF,0x3F,0x83,0x00,0x69,0x71,0x00,0x3F,0x3C,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,
0x6E,0x67,0x2D,0x63,0x6F,0x6D,0x6D,0x61,0x6E,0x64,0x20,0x76,0x65,0x72,0x73,0x69,0x8C,0xF9,

0xF9,0x81,0xEF,0x3F,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x74,0x61,0x6B,0x65,0x2D,0x70,
0x69,0x63,0x3D,0x22,0x59,0x45,0x53,0x22,0x20,0x7A,0x6F,0x6F,0x6D,0x3D,0x22,0x31,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x30,0x22,0x2F,0x3E,0x42,0x00,0x21,0x78,0x2D,0x62,0x74,0x2F,0x69,0x6D,0x61,
0x67,0x69,0x6E,0x67,0x2D,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,0x6E,0x67,0x2D,0x8C,0xF9,

0xF9,0x81,0xEF,0x19,0x69,0x6D,0x61,0x67,0x65,0x00,0x4C,0x00,0x06,0x06,0x01,0x80,0x50,0xF9};


PROGMEM char MCA25_CONFIG_640x480[] = {
0xF9,0x81,0xEF,0x3F,0x82,0x01,0x3B,0x01,0x00,0x03,0x49,0x01,0x35,0x3C,0x63,0x61,0x6D,0x65,
0x72,0x61,0x2D,0x73,0x65,0x74,0x74,0x69,0x6E,0x67,0x73,0x20,0x76,0x65,0x72,0x73,0x69,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x77,0x68,0x69,0x74,0x65,
0x2D,0x62,0x61,0x6C,0x61,0x6E,0x63,0x65,0x3D,
0x22,0x4F,0x46,0x46,0x22,
0x20,0x63,0x6F,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6C,0x6F,0x72,0x2D,0x63,0x6F,0x6D,0x70,0x65,0x6E,0x73,0x61,0x74,0x69,
0x6F,0x6E,0x3D,0x22,0x31,0x33,0x22,0x20,0x66,0x75,0x6E,0x2D,0x6C,0x61,0x79,0x65,0x72,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x3D,0x22,0x30,0x22,0x3E,0x3C,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,
0x6E,0x67,0x2D,0x66,0x6F,0x72,0x6D,0x61,0x74,0x20,0x65,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x67,0x3D,0x22,0x45,0x42,0x4D,0x50,0x22,0x20,0x70,0x69,0x78,0x65,0x6C,
0x2D,0x73,0x69,0x7A,0x65,0x3D,0x22,0x38,0x30,0x2A,0x36,0x30,0x22,0x20,0x63,0x6F,0x6C,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6F,0x72,0x2D,0x64,0x65,0x70,0x74,0x68,0x3D,0x22,0x38,0x22,0x2F,0x3E,
0x0D,0x0A,0x3C,0x74,0x68,0x75,0x6D,0x62,0x6E,0x61,0x69,0x6C,0x2D,0x66,0x6F,0x72,0x6D,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x61,0x74,0x20,0x65,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x45,
0x42,0x4D,0x50,0x22,0x20,0x70,0x69,0x78,0x65,0x6C,0x2D,0x73,0x69,0x7A,0x65,0x3D,0x22,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x31,0x30,0x31,0x2A,0x38,0x30,0x22,0x20,0x63,0x6F,0x6C,0x6F,0x72,0x2D,
0x64,0x65,0x70,0x74,0x68,0x3D,0x22,0x38,0x22,0x2F,0x3E,0x0D,0x0A,0x3C,0x6E,0x61,0x74,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x69,0x76,0x65,0x2D,0x66,0x6F,0x72,0x6D,0x61,0x74,0x20,0x65,0x6E,0x63,
0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x22,0x20,0x70,0x69,0x78,0x65,0x6C,0x2D,0x73,0x69,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x7A,0x65,0x3D,0x22,
0x31,0x36,0x30,0x2A,0x31,0x32,0x30,
0x22,0x2F,0x3E,
0x0D,0x0A,0x3C,0x2F,0x63,0x61,0x6D,0x65,0x72,0x61,0x2D,0x73,0x65,0x74,0x74,0x69,0x6E,0x8C,0xF9,
0xF9,0x81,0xEF,0x0B,0x67,0x73,0x3E,0x0D,0x0A,0xAF,0xF9};



unsigned char memcmp_P_mca(unsigned char* ptr1, const char* ptr2, unsigned char len) {

 while(len--) if(*ptr1++!=pgm_read_byte(ptr2++)) return 1;
 return 0;
}


void MCA25_Reset(void) {

 MCA25_RESET_LO();
 delay_ms(10);
 MCA25_RESET_HI();
}


unsigned char mca25_read_at_command(unsigned char *buffer) {

unsigned char cnt;
 
 for(cnt=0;cnt<MCA25_COMM_BUFFER_LEN;cnt++) {
   MCA25_STATUS_LED_ON();
   buffer[cnt] = USART_getchar();
   MCA25_STATUS_LED_OFF();
   if (buffer[cnt] == '\r' || !usart_timeout) break; 
 } 
 return cnt;
}


unsigned char mca25_read_mux_packet(unsigned char *buffer) {

 unsigned char cnt;

 for(cnt=0;cnt < MCA25_COMM_BUFFER_LEN;cnt++) {
 MCA25_STATUS_LED_ON(); 
   buffer[cnt] = USART_getchar();
   MCA25_STATUS_LED_OFF(); 
   if ((cnt>0 && buffer[cnt] == '\xF9') || !usart_timeout) {
     break; //we have finished out read.
   }
 }
 return cnt;
}


void mca25_send_data_ack(void) {

 USART_pstr_P(PSTR("\xF9\x81\xEF\x07\x83"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x03\xA6\xF9"));
}


char mca25_init(void) {

 MCA25_RESET_PORT_DIR=(1<<MCA25_RESET_PIN);
 MCA25_RESET_PORT=(unsigned char)~(1<<MCA25_RESET_PIN); 
 delay_ms(10);

 MCA25_Reset();
#if USE_WEBCAM_LEDS
	MCA25_RESET_PORT_DIR |=  (1<<MCA25_STATUS_PIN); 
	MCA25_RESET_PORT_DIR |=  (1<<MCA25_CLOCK_PIN); 
	MCA25_RESET_PORT_DIR |=  (1<<MCA25_ERROR_PIN); 
#endif 

 delay_ms(1);
 USART_Flush();
 mca25_read_at_command(mca25_buf);
 if (memcmp_P_mca(mca25_buf+2,PSTR("AT&F"),4)) return 1;
 USART_pstr_P(PSTR("\r\r\nOK\r\n"));
 mca25_read_at_command(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("AT+IPR=?"),8)) return 2;
 USART_pstr_P(PSTR("+IPR: (),(1200,2400,4800,9600,19200,38400,57600,460800)\r\n\r\nOK\r\n"));
 mca25_read_at_command(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("AT+IPR=460800"),13)) return 2;
 USART_pstr_P(PSTR("\r\nOK\r\n"));
 delay_ms(10);
 SET_USART_460800();
 delay_ms(1);
 USART_Flush();
 mca25_read_at_command(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("AT+CMUX=?"),9)) return 3;
 USART_pstr_P(PSTR("\r\r\n+CMUX: (0),(0),(1-7),(31),(10),(3),(30),(10),(1-7)\r"));
 mca25_read_at_command(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("AT+CMUX=0,0,7,31"),16)) return 4;
 USART_pstr_P(PSTR("\r\r\nOK\r\n"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x03\x3F\x01\x1C\xF9"),6)) return 5;
 USART_pstr_P(PSTR("\xF9\x03\x73\x01\xD7\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x23\x3F\x01\xC9\xF9"),6)) return 5;
 USART_pstr_P(PSTR("\xF9\x23\x73\x01\x02\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x03\xEF\x09\xE3\x05\x23\x8D\xFB\xF9"),10)) return 5;
 USART_pstr_P(PSTR("\xF9\x01\xEF\x0B\xE3\x07\x23\x0C\x01\x79\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x03\xEF\x09\xE1\x07\x23\x0C\x01\xFB\xF9"),11)) return 5;
 USART_pstr_P(PSTR("\xF9\x01\xEF\x09\xE1\x05\x23\x8D\x9A\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x23\xEF\x1B\x41\x54\x2A\x45\x41\x43\x53"
                             "\x3D\x31\x37\x2C\x31\x0D\xD1\xF9"),19)) return 5;
 USART_pstr_P(PSTR("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x23\xEF\x1D\x41\x54\x2B\x43\x53\x43\x43"
                             "\x3D\x31\x2C\x31\x39\x39\x0D\x35\xF9"),20)) return 5;
 USART_pstr_P(PSTR("\xF9\x21\xEF\x1B\x0D\x0A\x2B\x43\x53\x43\x43\x3A\x20\x45\x33\x0D\x0A\xB0\xF9"));
 USART_pstr_P(PSTR("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x23\xEF\x23\x41\x54\x2B\x43\x53\x43\x43\x3D"
                             "\x32\x2C\x31\x39\x39\x2C\x42\x39\x0D\xFB\xF9"),23)) return 5;
 USART_pstr_P(PSTR("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9"));
 USART_pstr_P(PSTR("\xF9\x81\x3F\x01\xAB\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x81\x73\x01\x60\xF9"),6)) return 5;
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x03\xEF\x09\xE3\x05\x83\x8D\xFB\xF9"),10)) return 5;
 USART_pstr_P(PSTR("\xF9\x01\xEF\x09\xE1\x05\x83\x8D\x9A\xF9"));
 USART_pstr_P(PSTR("\xF9\x81\xEF\x37\x80"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x1A\x10"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x02"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x46"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x13\xE3\x3D\x95\x45\x83\x74\x4A\xD7"));
 USART_pstr_P(PSTR("\x9E\xC5\xC1\x6B\xE3\x1E\xDE\x8E\x61\x82\xF9"));
 USART_pstr_P(PSTR("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9"));	  
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x3F\xA0\x00\x1F\x10\x00\x20\x00\xCB\x00"
                             "\x00\x00\x01\x4A\x00\x13\xE3\x3D\x95\x45\x83\x74\x4A"
                             "\xD7\x9E\xC5\xC1\x6B\xE3\x1E\xDE\x8E\xED\xF9"),37)) return 6;
 mca25_stat.busy=0;
 mca25_stat.skip_pic=0;
 mca25_stat.first_data = 1;
 return 0;
}


char mca25_configure(void) {

 mca25_read_mux_packet(mca25_buf); //read MUX packet
 mca25_pgm_send(MCA25_CONFIG_640x480,sizeof(MCA25_CONFIG_640x480));
 mca25_read_mux_packet(mca25_buf);
 if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x07\xA0\x00\x03\xC7\xF9"),9)) return 7;
 USART_pstr_P(PSTR("\xF9\x81\xEF\x2F\x83"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x17\x42"));
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x14\x78\x2D\x62\x74\x2F\x63\x61\x6D\x65\x72\x61"));  
 USART_pstr_P(PSTR("\x2D\x69\x6E\x66\x6F"));  
 USART_putchar('\x00');
 USART_pstr_P(PSTR("\x90\xF9"));
 mca25_read_mux_packet(mca25_buf);
 if (!memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x33\x79\x65\x72\x3D\x22\x31\x30\x22"
                              "\x2F\x3E\x3C\x2F\x63\x61\x6D\x65\x72\x61\x2D\x69"
                              "\x6E\x66\x6F\x3E\x00\xE4\xF9"),31)) return 7;
 return 0;
}


void mca25_grab_data(char *buffer, unsigned int *datalen, char *frametype) {

 unsigned int j=0;
 unsigned char togo=31;
 char rx=0;
 unsigned char state=0;
 unsigned char firstframe = 1;

 *datalen = 0;
 
 while( *datalen==0 || (j<*datalen) || state>99 || state == 12 ) {
   MCA25_STATUS_LED_ON(); 
   rx = USART_getchar();
   MCA25_STATUS_LED_OFF(); 
   switch(state) {

   case 0: 
     if (rx == 0xF9)
       state = 1;
     break;

    case 1:
      if (rx == 0x83)
        state=2;
      else if (rx == 0xF9) {
             state = 1; //this is the real start byte
           } else {
               state = 0; // we missed something, try again.
             }
      break;

    case 2:
      if (rx == 0xEF) {
        state = 3;	// packet ok
      } else {
          if (rx == 0xF9) {
            state = 1;
          } else {
             state = 0; // something went wrong -> retry
            }
        }
      break;

    case 3: 	
      togo = (rx-1)>>1; // (rx-1)/2
      if (firstframe==1)
        state = 10; //get frame info
      else
        state = 100; //grab data
      break;

    case 10:
      togo--;
      state = 11; 
      firstframe = 0;
      break;

    case 11:
      *datalen = (unsigned int)(rx<<8);
      togo--;
      state = 12;
      break;

    case 12:
      *datalen = (unsigned int)*datalen + (unsigned int)(rx) - 6; //substract the first
      if (*datalen > CAM_BUFFER_LEN) {
        *datalen = CAM_BUFFER_LEN;
      }
      togo--;
      state = 13;
      break;

    case 13:
      *frametype = rx;
      togo--;
      state = 14;
      break;

    case 14:
      togo--;
      state = 15;
      break;

    case 15:
      togo--;
      state = 100; //now sample data
      break;

    case 100:
      if (j<CAM_BUFFER_LEN) mca25_buf[j] = rx;
       else MCA25_ERROR_LED_ON(); //printf("ARGHHHHHHHHHHHHHHH\n");
      j++;
      togo--;
      if (togo == 0)
      state = 101;
      break;

    case 101:
      state = 102;
      break;

    case 102: 
    if (rx != 0xF9) {
      MCA25_ERROR_LED_ON();
    }
      state = 0;
      break;
   }
 }
}


void mca25_grab_jpeg(char res) {

 mca25_pgm_send(MCA25_START_JPG_1,sizeof(MCA25_START_JPG_1));
 switch (res) {
  
  case 1:   mca25_pgm_send(MCA25_START_JPG_320x240,sizeof(MCA25_START_JPG_320x240));
            break;
  
  case 2:   mca25_pgm_send(MCA25_START_JPG_640x480,sizeof(MCA25_START_JPG_640x480));
            break;
  
  default:  mca25_pgm_send(MCA25_START_JPG_160x120,sizeof(MCA25_START_JPG_160x120));
 }
 mca25_pgm_send(MCA25_START_JPG_2,sizeof(MCA25_START_JPG_2));
}


void mca25_start_image_grab(void) {

 unsigned char state;
 unsigned char datapos;
 unsigned char i;

 //grab 6 preview pictures:
 for (i=0; i<6; i++) {
   mca25_read_mux_packet(mca25_buf);
//   delay_ms(1);
   mca25_pgm_send(MCA25_START_CAPTURING,sizeof(MCA25_START_CAPTURING));
/*   delay_ms(18);
   mca25_pgm_send(MCA25_START_CAPTURING_2,sizeof(MCA25_START_CAPTURING_2));
//   delay_ms(60);
   mca25_pgm_send(MCA25_START_CAPTURING_3,sizeof(MCA25_START_CAPTURING_3));
//   delay_ms(1);*/
   USART_pstr_P(PSTR("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9"));	  
//   delay_ms(1);
   mca25_send_data_ack();
   state = 0;
   datapos = 0;
   while (state != 100) {
     mca25_read_mux_packet(mca25_buf); //read MUX packet
     switch (state) {

       case 0:
// wait for first packet, decode if is last data
// [F9 83 EF 3F 90 01 00 xx xx FD * ] F9 ]
// xx xx = C3 00 -> first 256 byte
// xx xx = 48 01 -> middle
// xx xx = 49 01 -> last data!

#if CAM_BUFFER_LEN == 256
         if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x3F\x90"),5) == 0) {
#else
// 512byte buf:
// 90 02 00 C3 00 00 
// 90 02 00 48 01 FD
// A0 01 10 49 01 0D
         if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x3F\x90\x02"),6) == 0) {
#endif
           if (mca25_buf[7] == 0xC3 && mca25_buf[8] == 0x00) {
//first frame:
             datapos = 1;
           } else if(mca25_buf[7] == 0x48 && mca25_buf[8] == 0x01) {
//middle
             datapos = 2;
           } else if(mca25_buf[7] == 0x49 && mca25_buf[8] == 0x01) {
//end:
             datapos = 3;
           } else if(mca25_buf[7] == 0x48 && mca25_buf[8] == 0x00) {
//end?
             datapos = 2;
           } else {
               //printf("buf7=%x, buf8=%x\n\n",buf[7],buf[8]);
             }
             state = 1;
//last data -> send ack!	
             mca25_send_data_ack();
           } else if (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x3F\xA0"),5) == 0) {
// F9 83 EF 3F A0 00 4C 49 00 49 00 
                  if(mca25_buf[7] == 0x49 && mca25_buf[8] == 0x00) {
//end when CAM_BUF_LEN = 256
                    datapos = 3;
           } else if(mca25_buf[7] == 0x49 && mca25_buf[8] == 0x01) {
//end when CAM_BUF_LEN = 512
                    datapos = 3;
           } else {
               //printf("buf7=%x, buf8=%x\n\n",buf[7],buf[8]);
             }
             state = 1;
//last data -> send ack!	
             mca25_send_data_ack();
             }
             break;

       case 1:
// wait for end of 256 Byte packet:
// [F9 83 EF 11 ** ** ** ** ** ** ** ** 3F F9 ]
#if CAM_BUFFER_LEN == 256
         if ( (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x11"),4) == 0) ||
              (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x1D"),4) == 0) ) {
#else
         if ( (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x21"),4) == 0) ||
              (memcmp_P_mca(mca25_buf,PSTR("\xF9\x83\xEF\x31"),4) == 0) ) {
#endif
           state =0;

           if (datapos == 3) {
//pic finished -> exit
             state = 100;
           }
         }
	 break;
      }
     }
//preview image #i has been grabbed.
   }
}


void mca25_pgm_send(PGM_P pointer, unsigned int len) {

 while(len--) USART_putchar(pgm_read_byte(pointer++));
}


/*
 typical usage:
 
 1) mca25_init(); 
    -> trigger a hardware reset and activates mux transfer

 2) mca25_configure(); 
    -> set up image format etc.

 3) mca25_start_image_grab();
    -> activate image grabbing, take a preview image

 4) mca25_grab_jpeg();
    -> activate jpg transfer

 5) mca25_grab_data();
    -> get x byte data

 6) while (){ mca25_send_data_ack(); mca25_grab_data(); ... }
    -> ack & grab loop

 => see mca25_copy_image_data_to_tcp_buffer() as an example ;)
 
*/


/*======================================================================
| copy the image data to the (tcp) data buffer, 
| buffer must be at least CAM_BUFFER_LEN byte long !
`======================================================================*/

unsigned char mca25_copy_data(char *buffer, unsigned int *len) {

 //unsigned int len = 0;
 unsigned char frametype = 0;

 *len=0;

 if(mca25_stat.first_data) {
   mca25_start_image_grab();
   mca25_grab_jpeg(2);
   mca25_stat.first_data = 0;
 } else { mca25_send_data_ack();
   }
 mca25_grab_data(buffer, len, &frametype); 
 if (*len == 0) *len = CAM_BUFFER_LEN;

 // last picture is XX SH SL CC 00 
 // CC = 0x48 -> more data (?)
 //    = 0x49 -> last data
 if (frametype!=0x48) {
 // this is important ! after the image grad
 // we need to do this! without this the camera
 // sometimes hangs while grabbing another image ... strange
 // reconfig mux (?)
   USART_pstr_P(PSTR("\xF9\x01\xEF\x0B\xE3\x07\x23\x0C\x01\x79\xF9"));
   mca25_stat.first_data = 1;
   return 0; // this is a smaller packet -> it was the last
 } else
   return 1; // this is a full packet -> there should be more (fixme)
}

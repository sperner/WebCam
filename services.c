/*****************************************************************************
*  Simple Application using uIP 0.9
*
*	Änderungen für Nutzung als Zutrittskontrolle
*	Passwortabfrage für Webseite
*	Schaltfunktion für Relais
*
*****************************************************************************/

#include "compiler.h"
#include "services.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "mca25.h"
#include "delay.h"
#include "uipopt.h"
#include "uip.h"
#include "uip_arp.h"
#include <string.h>
#include "servo.h" 

#define HTTP_NOGET   0
#define HTTP_GET     1
#define HTTP_FILE    2
#define RAW_FILE     3
#define HTTP_GET_404 4
#define HTTP_GET_PIC 5
#define HTTP_PIC     6
#define HTTP_SERVO   7
#define HTTP_GET_401 11

extern mca25_t mca25_stat;
extern unsigned char mca25_buf[CAM_BUFFER_LEN];

prog_char index_html[] = {
"<head>"
"<title>ATmega32 WebCam</title>"
"<style type=\"text/css\">"
"div{width: 660px;border: 1px solid black;background: #e0e0e0;padding: 2px;}"
"#t{font-size: 18px; border-bottom: 0; margin-top: 8px;}"
"#c{width: 660px;border: 1px solid black;background: #e0e0e0; padding: 2px;"
"text-align: center;height: 480px;border-top: 0;border-bottom: 0;}"
"#d{border: 1px solid black;}"
"#f{font-size: 14px;border-top: 0; text-align: right; padding-bottom: 5px;}"
"#r{ background: #c0c0c0; border: 1px solid black; padding-left: 10px; padding-right: 10px;" "text-decoration: none; margin-right: 10px; }"
"#r:hover{ background: #707070;}"
"#w{visibility: visible; border: 1px solid white; position: absolute; background: #000000; color: #ffffff; top:200px; left:430px; width:150px; height:50px; z-index:1; }"
"</style>"
"<script language=\"Javascript\" type=\"text/javascript\">"
"function wait(i){"
"if (document.getElementById) "
"document.getElementById(\"w\").style.visibility = (i==1?\"visible\":\"hidden\"); "
"}\n"
"function cam(){"
  "wait(1);"
	"if (document.getElementById) "
	"document.getElementById(\"d\").src = \"/cam?t=\"+Math.floor(100+Math.random()*899); "
	"setTimeout(\"wait(0);\",9000);"
"}\n"
"function cam2(val){"
	"wait(1);"
	"if (document.getElementById) "
	"document.getElementById(\"d\").src = \"/cam?v=\"+val+\"&t=\"+Math.floor(100+Math.random()*899); "
	"setTimeout(\"wait(0);\",9500);"
"}\n"
"</script>"
"</head>"
"<blockquote>\r\n"
"<blockquote>\r\n"
"<body bgcolor=\"#336699\" vlink=\"#000000\" alink=\"#000000\" link=\"#000000\" onLoad=\"wait(0);\">"
"<center></center>"
"<center>"
"<div id=\"t\">ATmega32 + MCA-25 + RTL8019</div>"
"<div id=\"c\">"
"<div id=\"w\"><br>loading...</div>"
"<img id=\"d\" name=\"cam\" src=\"picture.jpg\" height=\"480\" width=\"640\" alt=\"loading...\"></div>"
//"<img id=\"d\" name=\"cam\" src=\"cam.jpg\" height=\"240\" width=\"320\" alt=\"loading...\"></div>"
"<div id=\"f\">"
//"<a id=\"r\" href=\"javascript:cam();\">reload...</a>"
#if USE_SERVO
//"<input name=\"v\" value=\"%SER\" type=\"text\" length=\"3\" width=\"30\" onChange=\"javascript:cam2(this.value);\">"
/*"servo pos: "
"<select name=\"v\" onchange=\"javascript:cam2(this.value);\">"
"<option value=0>0</option>"
"<option value=25>25</option>"
"<option value=50>50</option>"
"<option value=75>75</option>"
"<option value=100>100</option>"
"<option value=125>125</option>"
"<option value=150>150</option>"
"<option value=175>175</option>"
"<option value=200>200</option>"
"<option value=225>225</option>"
"<option value=250>250</option>"
"</select>"
*/
#endif
"Servo : "
"<a id=\"r\" href=\"000\">125</a>"
"<a id=\"r\" href=\"025\">100</a>"
"<a id=\"r\" href=\"050\">75</a>"
"<a id=\"r\" href=\"075\">50</a>"
"<a id=\"r\" href=\"100\">25</a>"
"<a id=\"r\" href=\"125\">0</a>"
"<a id=\"r\" href=\"150\">25</a>"
"<a id=\"r\" href=\"175\">50</a>"
"<a id=\"r\" href=\"200\">75</a>"
"<a id=\"r\" href=\"225\">100</a>"
"<a id=\"r\" href=\"250\">125</a>"
" : "
"<a id=\"r\" href=\"\">reload</a>"
#if USE_SERVO
//"</form>"
#endif
"</div>"
"<div id=\"f\">"
"Relais : "
"<a id=\"r\" href=\"yaqwsx\">schliessen</a>"
"<a id=\"r\" href=\"cderfv\">oeffnen</a>"
"</div>"
"<a href=\"http://bigbabou.dyndns.info\">Homepage</a></center>"
"</body></html><center>"
"BigWebBabou, ATmega32 - 14,7456 MHz - HandyCam - Servo - uIP"
"</center></body></html>"
"\r\n\r\n%EOF" //%EOF = Ende des Files
};

/*
prog_char error_401[]={"<html><body bgcolor=\"white\"><center><h1>401 - unauthorized"
                       "</h1></center></body></html>\r\n\r\n"};
*/
prog_char error_404[]={"<html><body bgcolor=\"white\"><center><h1>404 - file not found"
                       "</h1></center></body></html>\r\n\r\n"};
/*
prog_char unauthorized[] = {
"HTTP/1.0 401 Unauthorized\r\n"
"Server: AVR_Small_Webserver\r\n"
"WWW-Authenticate: Basic realm=\"NeedPassword\""
"\r\nContent-Type: text/html\r\n\r\n"};
*/

u8_t chk4getend(void) {
 if (uip_datalen()<5) return 1;
 return !strncmp_P((const char*)uip_appdata+uip_datalen()-4,PSTR("\r\n\r\n"),4);
}


u8_t strncmp_PP(prog_char* ptr1, prog_char* ptr2, u8_t len)
{
 while(len--) if (pgm_read_byte(ptr1++)!=pgm_read_byte(ptr2++)) return 1;
 return 0;
}


void sendraw_P(prog_char* dataptr_P, u16_t dataleft)
{
 memcpy_P((u8_t*)uip_appdata,dataptr_P,(dataleft>uip_mss())?uip_mss():dataleft);
 uip_send(uip_appdata,dataleft);
}


prog_char* sendhtml_P(struct http_state* s)
{
 u16_t i;
 prog_char* ptr=s->dataptr_P;
 
 for (i=0; i<uip_mss(); i++) {
  if ((*(uip_appdata+i)=pgm_read_byte(ptr++)) =='%') {
    if (strncmp_PP((prog_char*)PSTR("EOF"),ptr,3) == 0) {ptr=(prog_char*)0; break;}
  }
 }
 if (i) uip_send(uip_appdata,i);
 return ptr;
}


void start_pic_trans(struct http_state* s,unsigned int *len) {
 if (chk4getend()) {
   if (mca25_stat.init) {
     // do something here - create a message for user
     uip_abort();
     return;
   }
   if (mca25_stat.busy) {
     uip_close();
     return;
   }
   s->state=HTTP_PIC;
   mca25_stat.busy=1;
   mca25_copy_data(mca25_buf,len);
   uip_send(mca25_buf,*len);
 }
}


void httpd(void) {
   
   static unsigned int len=0;
   static char ret=1;
  
   struct http_state *s;
   s = (struct http_state *)uip_conn->appstate;

   if(uip_connected()) {
     s->state=HTTP_NOGET;
     return;
   }
   
   if(uip_newdata()) {
     switch (s->state) {

     case HTTP_NOGET:
      if (strncmp_P((const char*)uip_appdata,PSTR("GET /"),5)) uip_abort();
/*
      if (uip_appdata[5]==' ') {
         s->state=HTTP_GET_401;
         if (chk4getend()) {
           s->dataptr_P=(prog_char*)error_401;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         }
	 uip_abort();
         break;  
      }
*/
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("sesam.html"),7)) {
         s->state=HTTP_GET;
         if (chk4getend()) {
           s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         }
         break;  
      }
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("picture.jpg"),7)) {
         
         s->state=HTTP_GET_PIC;
         start_pic_trans(s,&len);
         break;
      }
// schalten
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("yaqwsx"),3) ) {
           PORTB |= (1 << 4);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
	   if (!strncmp_P((const char*)uip_appdata+5,PSTR("cderfv"),3) ) {
           PORTB &= ~(1 << 4);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }

//SERVO TEST
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("000"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("000")+6);
    	   servo_prepare_pos(val); 
           servo_set_pos(0);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
	        if (!strncmp_P((const char*)uip_appdata+5,PSTR("025"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("025")+6);
			servo_prepare_pos(val); 
            servo_set_pos(25);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("050"),3)) {
		unsigned char val = (unsigned char)atoi(PSTR("050")+6);
			servo_prepare_pos(val); 
           servo_set_pos(50);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
	        if (!strncmp_P((const char*)uip_appdata+5,PSTR("075"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("075")+6);
			servo_prepare_pos(val); 
           servo_set_pos(75);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
	        if (!strncmp_P((const char*)uip_appdata+5,PSTR("100"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("100")+6);
			servo_prepare_pos(val); 
           servo_set_pos(100);
		   s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("125"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("125")+6);
			servo_prepare_pos(val); 
        servo_set_pos(125);
                    s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
	        if (!strncmp_P((const char*)uip_appdata+5,PSTR("150"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("150")+6);
			servo_prepare_pos(val); 
            servo_set_pos(150);
           s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("175"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("175")+6);
			servo_prepare_pos(val); 
            servo_set_pos(175);
           s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("200"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("200")+6);
			servo_prepare_pos(val); 
            servo_set_pos(200);
           s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }
      if (!strncmp_P((const char*)uip_appdata+5,PSTR("225"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("225")+6);
			servo_prepare_pos(val); 
            servo_set_pos(225);
           s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }

      if (!strncmp_P((const char*)uip_appdata+5,PSTR("250"),3) ) {
		unsigned char val = (unsigned char)atoi(PSTR("250")+6);
			servo_prepare_pos(val); 
            servo_set_pos(250);
           s->dataptr_P=(prog_char*)index_html;
           s->state=HTTP_FILE;
           s->nxtdatptr_P=sendhtml_P(s);
         break;
      }

      s->state=HTTP_GET_404;
      if (chk4getend()) {
        s->state=RAW_FILE;
        s->dataptr_P=(prog_char*)error_404;
        s->dataleft=sizeof(error_404);
        sendraw_P(s->dataptr_P,s->dataleft);
      }
      break;        
/*
     case HTTP_GET_401:
      if (chk4getend()) {
        s->dataptr_P=(prog_char*)error_401;
        s->state=HTTP_FILE;
        s->nxtdatptr_P=sendhtml_P(s);
      }
      uip_abort();
      break;
*/
     case HTTP_GET_404:
      if (chk4getend()) {
        s->state=RAW_FILE;
        s->dataptr_P=(prog_char*)error_404;
        s->dataleft=sizeof(error_404);
        sendraw_P(s->dataptr_P,s->dataleft);
      }
      break;        

     case HTTP_GET:
      if (chk4getend()) {
        s->dataptr_P=(prog_char*)index_html;
        s->state=HTTP_FILE;
        s->nxtdatptr_P=sendhtml_P(s);
      }
      break;

     case HTTP_GET_PIC:
      start_pic_trans(s,&len); 
      break;

     }
   }
 

   if(uip_rexmit()) {
     switch (s->state) {
     
     case HTTP_FILE:
      s->nxtdatptr_P=sendhtml_P(s);
      break;
    
     case RAW_FILE:
      sendraw_P(s->dataptr_P,s->dataleft);
      break;

     case HTTP_PIC:
      uip_send(mca25_buf,len);
      break;
     }    
     return;
   }

   if(uip_acked()) {
   
     switch (s->state) {
     
     case RAW_FILE:
      if(s->dataleft < uip_mss()) {
         uip_close();
	 break;
      }
      s->dataptr_P+=uip_conn->len;
      s->dataleft-=uip_conn->len;
      sendraw_P(s->dataptr_P,s->dataleft);
      break;
     
     case HTTP_FILE:
      if(s->nxtdatptr_P) {
        s->dataptr_P=s->nxtdatptr_P;
        s->nxtdatptr_P=sendhtml_P(s);
	break;
      }
      uip_close();
      break;

      case HTTP_PIC:
       if (ret) { 
         ret=mca25_copy_data(mca25_buf,&len);
         uip_send(mca25_buf,len);
         return;  
       }
       ret=1;
       mca25_stat.busy=0;
       uip_close(); 
       break; 
     }
   }
   
   if((uip_aborted()||uip_timedout()) && mca25_stat.busy) {
     while (mca25_copy_data(mca25_buf,&len)); 
     mca25_stat.busy=0;
     ret=1;
   }
   
}


void services_init(void) {
   
   uip_listen(HTONS(80));
}


void services_main(void) {
  
 switch(uip_conn->lport) {

  case HTONS(80):
   httpd();
   break;

  default:
   uip_abort();
   break;
  }  
}

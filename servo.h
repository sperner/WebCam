/**********************************************************
* simple servo control
* @Author: Simon Schulz [avr<AT>auctionant.de]
***********************************************************/
#ifndef _SERVO_H
 #define _SERVO_H

#define USE_SERVO	1
extern volatile unsigned char servo_pos;
extern volatile unsigned char servo_need_update;

extern void servo_init (void);
extern void servo_move(void);
extern void servo_prepare_pos(unsigned char);
extern void servo_set_pos(unsigned char);

#endif

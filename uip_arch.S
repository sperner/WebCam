/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip_arch.c,v 1.1 2004/05/09 00:24:47 Louis Exp $
 *
 */


#include <avr/io.h>

#define UIP_LLH_LEN 14
#define BUF uip_buf+UIP_LLH_LEN
#define IP_PROTO_TCP    6
#define ZERO r1

.extern uip_acc32
.extern uip_buf
.extern uip_appdata

.global uip_add32
.func uip_add32

uip_add32:
 movw ZL , r24
 ldd  r24, Z+3
 ldd  r25, Z+2
 ldd  r26, Z+1
 ld   r27, Z
 add  r24, r22
 adc  r25, r23
 adc  r26, ZERO
 adc  r27, ZERO
 sts  uip_acc32+3, r24
 sts  uip_acc32+2, r25
 sts  uip_acc32+1, r26
 sts  uip_acc32,   r27 
 ret
.endfunc  


.global uip_chksum
.func uip_chksum

uip_chksum:
 movw ZL , r24
 clr  r24
 clr  r25
 cpi  r22, 2
 cpc  r23, ZERO
 brcs uip_chksum_2
 movw r26, r22
 andi r26, 254
uip_chksum_1:
 ld   r23, Z+
 add  r24, r23
 ld   r23, Z+
 adc  r25, r23
 adc  r24, ZERO
 adc  r25, ZERO
 sbiw r26, 2
 brne uip_chksum_1
uip_chksum_2:
 sbrs r22, 0
 ret
 ld   r23, Z
 add  r24, r23
 adc  r25, ZERO
 adc  r24, ZERO
 adc  r25, ZERO
 ret
.endfunc


.global uip_ipchksum
.func uip_ipchksum

uip_ipchksum:
 ldi  r22, 20
 ldi  r23, 0
 ldi  r24, lo8(BUF)
 ldi  r25, hi8(BUF)
 rjmp uip_chksum
.endfunc


.global uip_tcpchksum
.func uip_tcpchksum

uip_tcpchksum:
 ldi  r22, 20
 ldi  r23, 0
 ldi  r24, lo8(BUF+20)
 ldi  r25, hi8(BUF+20)
 rcall uip_chksum
 movw r18, r24		// -> hsum
 lds  r24, BUF+3	// len[1]
 lds  r25, BUF+2	// len[0]
 sbiw r24, 20
 movw r20, r24
 sbiw r24, 20
 movw r22, r24
 lds  r24, uip_appdata
 lds  r25, uip_appdata+1
 rcall uip_chksum
 add  r24, r18
 adc  r25, r19
 lds  r22, BUF+12
 lds  r23, BUF+13	// srcipaddr[0]
 adc  r24, r22
 adc  r25, r23
 lds  r22, BUF+14
 lds  r23, BUF+15	// srcipaddr[1]
 adc  r24, r22
 adc  r25, r23
 lds  r22, BUF+16
 lds  r23, BUF+17	// destipaddr[0]
 adc  r24, r22
 adc  r25, r23
 lds  r22, BUF+18
 lds  r23, BUF+19	// destipaddr[1]
 adc  r24, r22
 adc  r25, r23
 ldi  r23, IP_PROTO_TCP
 adc  r24, ZERO
 adc  r25, r23
 adc  r24, r21
 adc  r25, r20
 adc  r24, ZERO
 adc  r25, ZERO
 ret
.endfunc

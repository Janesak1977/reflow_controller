/**
 * Copyright (c) 2012, Lawrence Leung
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   - The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OVENCON_H
#define OVENCON_H


#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

extern void fault(void);
extern void thermocouple_fault(int16_t result);
extern void debugmsg(PGM_P  pmsg);
extern void oven_update_120hz(void);
extern void oven_update_4hz(void);



// Thermocouple settings

// Do we have 2 thermocouple chips?  If not, top=bottom
//#define BOTTOM_THERM

// Device level moving average
//#define TEMP_AVERAGING
//#define AVERAGE_BITS 2
//#define AVERAGE (1<<AVERAGE_BITS)

#ifndef BOTTOM_THERM
#define DEVICES 1
#else
#define DEVICES 2
#endif


//#define DEBUG


#endif

/**
 * Copyright (c) 2011, Daniel Strother < http://danstrother.com/ >
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

#include "max6675.h"
#include "ovencon.h"

#ifdef TEMP_AVERAGING
int16_t temps[DEVICES][AVERAGE];
#endif

static void _max6675_select(uint8_t device, uint8_t cs)
{
    switch(device)
    {
        // PB0
        case 0:
        	DDRB |= _BV(0);

            if(cs)  PORTB &= ~(_BV(0)); // active-low
            else    PORTB |= _BV(0);

            break;
        // PC6
        case 1:
#ifndef BOTTOM_THERM
            DDRC |= _BV(6);
        	if(cs)  PORTC &= ~(_BV(6)); // active-low
            else    PORTC |= _BV(6);
#endif
            break;
    }

}

void max6675_setup(void)
{
    uint8_t i,j;

    // enable SPI; polled; MSB-first; Master; idle: low; sample: div: 32 (.5 MHz clock)
    SPCR    = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
    SPSR    = _BV(SPI2X);

    // set chip-selects inactive and initialize averages
    for(i=0;i<DEVICES;++i)
    {
        _max6675_select(i,0);

#ifdef TEMP_AVERAGING
        for(j=0;j<AVERAGE;++j){
            temps[i][j] = 100; // 25C
        }
#endif
    }

    // set SPI pin directions
    DDRB    &= ~(_BV(3));       // MISO as input
    DDRB    |= _BV(1) | _BV(2); // SCLK and MOSI as outputs


    DDRB |= _BV(5) | _BV(6); // enable PWM (maybe this will make my timer work?)
}


    // start initial conversion
void max6675_start(void){
    uint8_t i;
    for(i=0;i<DEVICES;i++)
    {
        _delay_ms(2);
        _max6675_select(i,1);
        _delay_ms(2);
        _max6675_select(i,0);
        _delay_ms(220);
    }
}

int16_t max6675_read(uint8_t device)
{ 
    int16_t result;
    int16_t avg;
    uint8_t i;
    
    cli();

    // clear
    result=SPIF;
    result=SPDR;
    SPDR = 0xFF;

    _max6675_select(device,1);
    _delay_ms(2);

    // read MSbyte

    loop_until_bit_is_set(SPSR,SPIF);
    result = SPDR;
    result <<= 8;

    // read LSbyte
    SPDR = 0xFF;
    loop_until_bit_is_set(SPSR,SPIF);
    result |= SPDR;

    // de-select device (starts new conversion)
    _max6675_select(device,0);

    sei();

    // check for open/shorted line or open-thermocouple flag
    if( result == 0x0000 || result == 0xFFFF || result & (1<<2) ) {
        result = 0xFFFF;
        thermocouple_fault(result);
        return result;
    }

    // result is in upper 13 bits (12 real bits; MSbit is dummy sign (always 0))
    result >>= 3;

    // check range (5-300 degrees)
    if( result < 10 || result > 1200) {
        thermocouple_fault(result);
        result = 0x0FFF;
        return result;
    }

    // average
    avg = result;

#ifdef TEMP_AVERAGING
    temps[device][0] = result;

    for(i=1;i<AVERAGE;i++)
    {
    	avg += temps[device][i];
        temps[device][i] = temps[device][i-1];
    }

    avg >>= AVERAGE_BITS;
#endif

    return avg;
}


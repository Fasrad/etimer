/*-------|---------|---------|---------|---------|---------|---------|---------|
etimer.c	

"f-stop" enlarger timer with exponential settings

refer to etimer.pdf for hardware details

by chaz miller for ATMEGAxx8 set at 1MHz running at 5V. 

This is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3 or any later
version. This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
******************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "./lut.h"

#define start_timer  TCCR1|=(1<<CS11);runflag=1;
#define stop_timer   TCCR1&=(1<<CS12|1<<CS11|1<<CS10);runflag=0; 
#define start PORTB&(1<<2)
#define cancel PORTB&(1<<1)
#define debounce 10 

void delay(uint16_t);
void blink (uint8_t);
void adc_init(void);
uint16_t adc_read(uint8_t);

int main(){
    /****************************************
    *****setup stuff*************************
    ****************************************/
    uint8_t runflag; 

    //8-bit timer2 for button debounce
    TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20);    //CPU/1024; 2Hz PWM

    //16-bit Timer 1 set as output PWM on OC1B PB2 (Arduino pin 10) p.115
    //noninverting phase correct, CTC-PWM hybrid mode p135 
    TCCR1A = (1<<COM1B1)|(1<<WGM11)|(1<<WGM10); 
    OCR1A = 0x0FF0;             //sets pwm TOP 

    adc_init();

    /****************************************
    *****main loop***************************
    ****************************************/
    for(;;){  
        //poll cancel button
        if(!(cancel)){                     
            if(TCNT2>debounce){               
                TCNT2=0;
                //fine dial is 2 stops not 8 hence '>>'
                OCR1B=pgm_read_byte(
                    &ocr_LUT[ adc_read(0) + adc_read(1)>>2 ]
                );
                TCNT1=0;
            }else{
                TCNT2=0;
            }
        }
        //poll start button
        if(!(start)){
            if(TCNT2>debounce){
                TCNT2=0;
                if(runflag){
                    stop_timer;
                }
                else{
                    start_timer;
                }
            }else{
                TCNT2=0;
            }
        }
   } //infty
}//main
void adc_init(void){
    //ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0); //125kHz @ 1MHz clock, page 264
    ADCSRA = (1<<ADEN)|(1<<ADPS2); //62kHz @ 1MHz clock, page 264
    ADMUX |= (1<<REFS0);           //AREF=Avcc (5v)
    //ADMUX |= (1<<ADLAR);           //left align for 8-bit operation
    ADCSRA |= (1<<ADEN); 
}
uint16_t adc_read(uint8_t me){    //expects ADMUX register value; blocking
    ADMUX &= 0xF0;
    ADMUX |= me;
	ADCSRA |= (1<<ADSC); 
	while(ADCSRA & (1<<ADSC)); 
	return ADCW;
}
/*
Set a bit
 bit_fld |= (1 << n)

Clear a bit
 bit_fld &= ~(1 << n)

Toggle a bit
 bit_fld ^= (1 << n)

Test a bit
 bit_fld & (1 << n)
 */

/*-------|---------|---------|---------|---------|---------|---------|---------|
etimer.c	

enlarger timer with two channels 

refer to etimer.pdf for hardware details

by chaz miller for ATMEGAxx8 set at 1MHz running at 5V. 

This is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3 or any later
version. This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
******************************************************************/

#include <avr/io.h>
#define stop_timer   TCCR12&=(1<<CS12|1<<CS11|1<<CS10);runflag=0; 
#define start_timer  TCCR12|=(1<<CS11);runflag=1;
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
    adc_init();
    //16-bit timer1 for enlargeage
    //8-bit timer0 for debounce

    uint8_t runflag; //flag
    uint16_t course; //course pot 
    uint16_t fine;   //fine pot 
    uint16_t ocr;    //timer OCR

    /****************************************
    *****main loop***************************
    ****************************************/
    for(;;){  
        //poll cancel button
        if(!(cancel)){                     
            if(TCNT0>debounce){               
                TCNT0=0;
                course=adc_read(0);    //course dial: 8 stops 
                fine=adc_read(1)>>2;   //fine dial: 2 stops hence '>>'
                exp=knobify(course)+knobify(fine);  //knob LUT  mapping
                OCR1B=expify(exp);                  //massive LUT of doom
                TCNT1=0;
            }else{
                TCNT0=0;
            }
        }
        //poll start button
        if(!(start)){
            if(TCNT0>debounce){
                TCNT0=0;
                if(runflag){
                    stop_timer;
                }
                else{
                    start_timer;
                }
            }else{
                TCNT0=0;
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
uint16_t adc_read(uint8_t me){    //expects ADMUX register value
    uint16_t ad_bucket=0;
    ADMUX &= 0xF0;
    ADMUX |= me;
    for (int i=0; i<16; i++){
	ADCSRA |= (1<<ADSC); 
	while(ADCSRA & (1<<ADSC)); 
	ad_bucket += ADCW;
    }
    return (ad_bucket>>2); //12 bits oversampled
}
uint16_t knobify(uint16_t me){
    //take knob value, return mapped exponential 
    uint8_t knob_LUT[1024]{
    }
}
uint16_t expify(uint16_t me){
    //take exponent, return OCR value 
    uint8_t ocr_LUT[16384]{
    }
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

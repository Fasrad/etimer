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
    //8-bit time2 pwm for white light

    uint8_t runflag; //flag
    uint16_t base;   //course pot 
    uint16_t fine;   //fine pot 
    uint16_t ocr;    //timer OCR

    /****************************************
    *****main loop***************************
    ****************************************/
    for(;;){  
        OCR2=adc_read(2)>>2;
        //poll cancel button
        if(!(cancel)){                     
            if(TCNT0>debounce){               
                TCNT0=0;
                //read dials ADCs
                base=adc_read(0); //0-1023 needs logified
                //<< to subtract a stop from base
                //add fine exposure; 0 on knob is really +1 stop 
                fine=adc_read(1); //0-1023 needs logified; added somehow

                //compute timer blob(s)
                //load timer registers

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
/*
    //8 bit Timer 0 is used by delay().
    TCCR0A = 0;                //standard timer mode (page 103)
    TCCR0B = 2;                //fcpu / 1
void delay(uint16_t me){    //at 1MHz, each unit is 2.55us. 1ms is 4units. 
    while(me){
	while(TCNT0 < 128){}
	me--;
	while(TCNT0 > 128){}
    }
}
void blink (uint8_t me){
    for (int i=0; i<me; i++){
	PORTB |= (1<<5);
	delay(200);
	PORTB &= ~(1<<5);
	delay(200);
    }
    delay(500);
}
Set a bit
 bit_fld |= (1 << n)

Clear a bit
 bit_fld &= ~(1 << n)

Toggle a bit
 bit_fld ^= (1 << n)

Test a bit
 bit_fld & (1 << n)
*/ 

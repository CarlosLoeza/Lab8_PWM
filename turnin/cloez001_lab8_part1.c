/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


enum pwm_States{pwm_on, pwm_Off, pwm_Hold}pwm_State;;

unsigned tmpA;

void set_PWM(double frequency) {
    static double current_frequency; 

    if(frequency != current_frequency){
	if(!frequency) {TCCR3B &= 0x08;} 
	else{TCCR3B |= 0x03;}

	if(frequency < 0.954) {OCR3A = 0xFFFF;}
	else if(frequency > 31250) {OCR3A = 0x0000;}
	else{OCR3A = (short)(8000000/(128*frequency)) - 1;}
	TCNT3 = 0;
	current_frequency = frequency;		
    }
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void Sound(){
    switch (pwm_State){
	case pwm_on:
	    if(tmpA & 0x01){
		set_PWM(261.63);
		pwm_State = pwm_Hold;
	    }
	    else if(tmpA & 0x02){
		set_PWM(293.66);
		pwm_State = pwm_Hold;
	    }
	    else if(tmpA & 0x04){
		set_PWM(329.63);
		pwm_State = pwm_Hold;
	    }

	    break;

	case pwm_Off:
	    set_PWM(0);
	    pwm_State = pwm_on;
	    break;
		
	case pwm_Hold:
	    while(tmpA) //HOLD
	    {
		tmpA = ~PINA & 0x0F;
	    }

	    pwm_State = pwm_Off;
	    break;
			
	default:
	    pwm_State = pwm_on;
	    break;
    }
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

	
    PWM_on();
    set_PWM(0);

    pwm_State = pwm_on;
    while (1) 
    {
	tmpA = ~PINA & 0x0F;
	Sound();
    }
}


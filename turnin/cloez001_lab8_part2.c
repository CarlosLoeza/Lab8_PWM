/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *     
 *      Demo link: 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


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

enum pwm_States{pwm_SMStart, pwm_On, pwm_Sound, pwm_Up, pwm_Down , pwm_Hold}pwm_State;
unsigned char i;
unsigned char tmpA;
unsigned char on;
	
Sound(){
    switch(pwm_State){
	case pwm_SMStart:
	    if(tmpA & 0x01){
		pwm_State = pwm_On;
	     }
	     else if(tmpA & 0x02){
		pwm_State = pwm_Down;
	     }
	else if(tmpA & 0x04){
		pwm_State = pwm_Up;
	} 
	break;

	case pwm_On:
	    if(on == 0){
		pwm_State = pwm_Sound;
		on = 1;
	    }
	    else if(on == 1){
		set_PWM(0);
		pwm_State = pwm_Hold;
		on = 0;
	    }

	    break;
	case pwm_Sound:
	    switch(i){
		case 1: set_PWM(261.63); break;
		case 2: set_PWM(293.66); break;
		case 3: set_PWM(329.63); break;
		case 4: set_PWM(349.23); break;
		case 5: set_PWM(392.00); break;
		case 6: set_PWM(440.00); break;
		case 7: set_PWM(493.88); break;
		case 8: set_PWM(523.25); break;
		default: break;
	    }
	    pwm_State = pwm_Hold;

	    break;
			
	case pwm_Up:
	    if(i != 8){
		i++;	
	    }
	    pwm_State = pwm_Sound;	
	    break;

	case pwm_Down:
	    if(i != 1){
		i--;	
	    }
	    pwm_State = pwm_Sound;					
	    break;

	case pwm_Hold:
	    while(tmpA){ tmpA = ~PINA & 0x0F;}
		pwm_State = pwm_SMStart;
		break;
	}
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x4F; PORTB = 0x00;	

    pwm_State = pwm_SMStart;

    i = 1;
    on = 0;
    PWM_on();
    set_PWM(0);
    while (1) {
	tmpA = ~PINA & 0x0F;
	Sound();
    }
}

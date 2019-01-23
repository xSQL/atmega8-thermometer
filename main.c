#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

const char SEGMENTE[] = {
   0b00101000,   //0
   0b11101110,   //1
   0b01011000,   //2
   0b01001010,   //3
   0b10001110,   //4
   0b00001011,   //5
   0b00001001,   //6 
   0b01101110,   //7
   0b00001000,   //8 
   0b00001010,   //9
   0b11110111,   // dot
   0b11011111,   // minus
   0xFF                 //OFF
 };
unsigned char digit_array[] = {0, 0, 0};
unsigned char digit_counter = 0;


ISR(TIMER0_OVF_vect) {
    PORTB = 0xFF;
    PORTD = 1<<(digit_counter + 5) ;
    PORTB = SEGMENTE[digit_array[digit_counter]];

    if(++digit_counter>2)
        digit_counter = 0;
    TCNT0 = 0;
}

void init_timer (void)
{
    TIMSK=(1<<TOIE0);                    //Enable timer overflow interrupt
    TCCR0=(0<<CS00)|(1<<CS01)|(0<<CS02); //Prescaller = /1
}

unsigned int getADC(void) //Считывание АЦП
{
    unsigned int v;
    ADCSRA|=(1<<ADSC);	//Начать преобразование
    while ((ADCSRA&_BV(ADIF))==0x00) {
    }
    v=(ADCL|ADCH<<8);
    return v;
}



void convert_data(unsigned int x) {
    if(x>99) {
        digit_array[2] = x%10;
        digit_array[1] = (x/10)%10;
        digit_array[0] = x/100;
    } else if(x>9) {
        digit_array[2] = x%10;
        digit_array[1] = x/10;
        digit_array[0] = 12;
    } else if(x<10) {
        digit_array[2] = x;
        digit_array[1] = 12;
        digit_array[0] = 12;
    }
}


int main()
 { 
    DDRB = 0xFF;
    DDRD = 0xFF;
    init_timer(); 			//Init timer
    sei();				//Interrupt enable
    ADCSRA=(1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);	
    //Включаем АЦП, тактовая частота бреобразователя =/8 от тактовой микроконтроллера
    ADMUX=(1<<REFS1)|(1<<REFS0)|(0<<MUX0)|(0<<MUX1)|(0<<MUX2)|(0<<MUX3);			
    //Внутренний источник опорного напряжения Vref=2,56, входом АЦП является PC0

    // Write your code here
    while(1) {
        //convert_data((int)getADC()*256/1024);
        
        convert_data((int)((getADC()*2.56/1024)/0.0041));
        //Conver data to codes
        _delay_ms(500);
    }
    return 0;
 }

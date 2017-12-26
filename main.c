/*
 * File:   main.c
 * Author: Trent

 *
 * Created on May 19, 2015, 6:47 PM
 */


#include <xc.h>

#include <stdio.h>
#include <stdlib.h>


#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)





#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif

char counter0;
char counter1;
char counter2;
char counter3;
static int counter4; //used for software a-sync uart transmission
char tempCounter;
char receiveByte;
char transmitByte;
char txByteBuff;
char rxByteBuff;

bit incomingData;
bit softTXflag;
bit outgoingData;
bit txStat;
bit checkRxByte;

void txByte(char byte);
void interrupt isrRoutine(void);
void putch(char byte);
char getByte();
void primaryMenu();


char getByte(){
    
    
    if((counter2 == 9) && (INTF == 1))
    {
        
        //rxByteBuff = receiveByte;
        return rxByteBuff;
        
    }else{
        
        return 0;
        
    }
    
    
    
    
}

void primaryMenu(){
    
    
    printf("Welcome to the Bus Agent main menu! \n\r");
    printf("pick a number from the menu below");
    printf("1 - Hardware Uart\r\n");
    printf("2 - Hardware SPI\r\n");
    printf("3 - Hardware I2C\r\n");
    printf("4 - Hardware PWM\r\n");
    printf("5 - Software PWM\r\n");
    printf("6 - Hardware GPIO\r\n");
    while(!checkRxByte);
    if(rxByteBuff == 1){
        
        printf("You selected option 1");
        checkRxByte = 0;
    }    
        
        
        
    
    
}





void txByte(char byte) {

    txStat = 1;
    txByteBuff = byte;
    
    RB4 = 0;
    //send start byte
    for (int i = 0; i < 10; i++) {
        __delay_us(5);
        NOP();
        NOP();
    }

    for (int j = 0; j <= 7; j++) {

        if (txByteBuff & 0x01) {

            RB4 |= 1;
            txByteBuff >>= 1;

            for (int i = 0; i < 10; i++) {
                __delay_us(5);
            }



        } else {

            RB4 = 0;
            txByteBuff >>= 1;

for (int i = 0; i < 9; i++) {
        __delay_us(6);
        NOP();

    }
       __delay_us(1);     

        }



    }
    
    __delay_us(1); 
    RB4 = 1;
    txByteBuff = 0;
    txStat = 0;
}




void putch(char byte){
    
    __delay_ms(1);
    txByte(byte);
    
    
}


    void interrupt isrRoutine(void) {

        //if readpin set and counter1 = 12 and if counter2 is less then 8
        //read pin and write to byte
        //counter2++;
        //counter1 = 0

        if (incomingData) {
            //check if enough time has passed to receive a bit 
            if (counter1 == 4 && counter2 == 0) {


                counter1 = 0;
                counter2++;

            } else if (counter1 == 8 && (counter2 >= 1 && counter2 <= 9)) {

                if (counter2 >= 1 && counter2 <= 8) {

                    if (RB0 == 1) {
                        receiveByte = receiveByte >> 1;
                        receiveByte |= 0x80;




                    } else {

                        receiveByte = receiveByte >> 1;



                    }




                    counter1 = 0;

                }


                if (counter2 == 9) {
                    //checkRxByte = 1;
                    rxByteBuff = receiveByte;
                    checkRxByte = 1;
                    outgoingData = 1;
                    //done receiving byte
                    incomingData = 0;
                    // re enable inte to receive more bytes
                    INTE = 1;

                    INTF = 0;
                    //reset counter2 (keeps track of bit reception number
                    counter2 = 0;
                    counter1 = 0;
                    INTCON &= ~0x24;
                    //           TMR0IE = 1;
                    //         TMR0IF = 1;

                }

                counter2++;


                //check it bit catching mechanism has ran at least 8 times.
            }



        }

        if (TMR0IE && TMR0IF) {

            //TMR0 = 240;

#asm
            movlw 243
                    movwf TMR0

#endasm


                    counter0++;
            counter1++;

            TMR0IF = 0;


        }

        if (INTE && INTF) {

            INTCON |= 0x36;
            incomingData |= 1;
            counter1 = 0;

            //  TMR0IE = 1;
            // TMR0IF = 1;


            //disable edge interrupts temporarily for bit reception
            INTE = 0;

            INTF = 0;

            counter2 = 0;

            receiveByte = 0;
        }





        //if rbo goes low, delay 104 ucsec 8 times and read pin each time

        //if rbo go low
        //set readPin bit






    }

    void main(void) {


        T0CS = 0;

        OPTION_REG |= 0xA8;
        OPTION_REG &= ~0x20;

        /*PSA = 1; // This assigns the pre Scaler to the WDT
        PS2 = 0; //prescaler of 1:1
        PS1 = 0; //prescaler of 1:1
        PS0 = 0; //prescaler of 1:1 */
        //END SEQUENCE
        /*TMR0 = 250;
        TMR0IE = 1; //enable timer0 interrupts
        TMR0IF = 0; //reset the timer 0 interrupt flag
        GIE = 1; // if not enabled. enable global interrupts
    
         */

        INTCON = 0xE0;


        TRISB |= 0b10000001;
        TRISB &= ~0b00000000;
        //TRISB4 = 0;
        //RB4 = 0;


        ////// enable RB0 interrupt on change
        //TRISB0 = 1;

        //enable transmit pin for software uart 0
        //TRISB1 = 0;

        //enable weak pull ups
        //nRBPU = 1; SAME AS LINE BELOW
        //OPTION_REG |= 0x80;

        //choose falling edge signal as cause of interrupt

        //INTEDG = 0;





        //enable port change interrupts for RB0

        INTE |= 1;

        //reset flag for intf
        INTF = 0;
        //INTF = 0; SAME AS LINE BELOW
        //INTCON |= 0x02;

#asm 
        bcf INTCON, 1;
#endasm;

        TMR0IE = 0;
        TMR0IF = 0;


        while(1){

    primaryMenu();
            

        }







    }

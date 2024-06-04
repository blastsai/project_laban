#include "MKL46Z4.h"     
#include "system.h"       
#include <stdio.h>

volatile int led_green_toggle = 0;
volatile int led_red_toggle = 0;


void init_system(void) {
    // Cap xung clock de khoi tao
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTC_MASK;

    // PTD5 là output dèn LED xanh
    PORTD->PCR[5] |= (1 << 8); // Set MUX to 1 to select GPIO function
    GPIOD->PDDR |= (1 << 5); // Set as output

    // PTE29 là output dèn LED do
    PORTE->PCR[29] |= (1 << 8); // Set MUX to 1 to select GPIO function
    GPIOE->PDDR |= (1 << 29); // Set as output

    // Cau hình chân PTC3  SW1  
    PORTC->PCR[3] |= (1 << 8); // Set MUX to 1 to select GPIO function
    PORTC->PCR[3] |= (1 << 1) | (1 << 0); // Enable pull-up resistor
    PORTC->PCR[3] |= (0b1010 << 16); // Select interrupt on falling edge
		
		// Cau hình chân PTC12  SW3
		PORTC->PCR[12] |= (1 << 8); // Set MUX to 1 to select GPIO function
    PORTC->PCR[12] |= (1 << 1) | (1 << 0); // Enable pull-up resistor
    PORTC->PCR[12] |= (0b1010 << 16); // Select interrupt on falling edge
		
		// Thiet lap uu tien ngat ( NEU CODE KHONG CHAY COMMENT DONG NAY)
		NVIC_SetPriority(PORTC_PORTD_IRQn,1);
		NVIC_SetPriority(SysTick_IRQn,2);
		
		
		 // Cau hình SysTick timer
    SysTick_Config(SystemCoreClock / 1000); // Tao ngat moi 1ms
		// SysTick Timer voi dong ho he thong và bat ngat
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
		
		
		NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);// CLEAR NGAT
		NVIC_EnableIRQ(PORTC_PORTD_IRQn); // Mo ngat cho PORTC
		
}


void SysTick_Handler(void) {
    static int count_green = 0;
    static int count_red = 0;
    
    // dèn LED xanh (1 Hz)
    count_green++;
    if (count_green >= 1000) { // 1 s
        count_green = 0;
        led_green_toggle = 1;
    }
    
    //dèn LED do (2 Hz)
    count_red++;
    if (count_red >= 500) { // 500 ms
        count_red = 0;
        led_red_toggle = 1;
    }
		 control_LEDs();// Cap nhat trang thái LED tu SysTick Handler
}

void control_LEDs(void) {
    if (system_state == 1) {
        GPIOE->PSOR = (1 << 29); // Tat led do
        // led xanh nhay 1Hz
        if (led_green_toggle) {
            GPIOD->PTOR = (1 << 5);  // Ðao trang thái LED xanh
            led_green_toggle = 0;
        }
			}
				else {  
        GPIOD->PSOR = (1 << 5);  // Tat LED xanh
        if (led_red_toggle) {
            GPIOE->PTOR = (1 << 29); // Ðao trang thai led do
            led_red_toggle = 0;
        }
    }
				
}


void PORTC_PORTD_IRQHandler(void) {
    // Kiem tra nut SW1 
    if (PORTC->ISFR & (1 << 3)) {
        if(system_state == 0){
					system_state = 1;
				}
				else {
					system_state = 0;
		}
				PORTC->ISFR |= (1 << 3);	// xoa co ngat
				
	}
		// Kiem tra nut SW3		
		if (PORTC->ISFR & (1 << 12)) {
        SCB->AIRCR = 0x05FA0004;
        PORTC->ISFR |= (1 << 12); // xoa co ngat
    }
	}








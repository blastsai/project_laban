
#include "MKL46Z4.h"
#include "lcd.h"

// khoi tao mô-dun LCD
uint32_t LCD_Init(void)
{
    // khoi tao xung clock IRCLK cho phép hoat dong trong che do dung
    MCG->C1  |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;

    // Bat clock cac cong de cau hinh chan lcd
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // Clock cho  LCD
    SIM->SCGC5 |= SIM_SCGC5_SLCD_MASK; 

    
    LCD->GCR |= LCD_GCR_PADSAFE_MASK; // Bat che do an toan de tranh xung dot khi config
    LCD->GCR &= ~LCD_GCR_LCDEN_MASK; // Tat LCD

		// Cau hình các chân GPIO thành chân LCD
		PORTB->PCR[7] = PORT_PCR_MUX(0u);
    PORTB->PCR[8] = PORT_PCR_MUX(0u);
		PORTB->PCR[10] = PORT_PCR_MUX(0u);
    PORTB->PCR[11] = PORT_PCR_MUX(0u);
    PORTB->PCR[21] = PORT_PCR_MUX(0u);
		PORTB->PCR[22] = PORT_PCR_MUX(0u);
		PORTB->PCR[23] = PORT_PCR_MUX(0u);
		PORTC->PCR[17] = PORT_PCR_MUX(0u);
		PORTC->PCR[18] = PORT_PCR_MUX(0u);
		PORTD->PCR[0] = PORT_PCR_MUX(0u);
		PORTE->PCR[4] = PORT_PCR_MUX(0u);
    PORTE->PCR[5] = PORT_PCR_MUX(0u);
		
   
    
	
		// Cau hinh chung LCD
    LCD->GCR = LCD_GCR_RVTRIM(0x00) |
               LCD_GCR_CPSEL_MASK |
               LCD_GCR_LADJ(0x03) |
               LCD_GCR_VSUPPLY_MASK |
               LCD_GCR_ALTDIV(0x00) |
               LCD_GCR_SOURCE_MASK |
               LCD_GCR_LCLK(0x01) |  
               LCD_GCR_DUTY(0x03);
		// tan so nhay LCD
    LCD->AR = LCD_AR_BRATE(0x03); 

    // Xóa loi LCD
    LCD->FDCR = 0U;
	
		// Cau hình các chân duoc bat cho LCD
    LCD->PEN[0] =	LCD_PEN_PEN(1u << 7)  |   // P7
                  LCD_PEN_PEN(1u << 8)  |   // P8
                  LCD_PEN_PEN(1u << 10) |   // P10
                  LCD_PEN_PEN(1u << 11) |  	// P11
									LCD_PEN_PEN(1u << 17) |   // P17
									LCD_PEN_PEN(1u << 18) |   // P18
									LCD_PEN_PEN(1u << 19);   	// P19

    LCD->PEN[1] = LCD_PEN_PEN(1u << 5)  |   // P37
                  LCD_PEN_PEN(1u << 6)  |   // P38
									LCD_PEN_PEN(1u << 8)  |   // P40
                  LCD_PEN_PEN(1u << 20) | 	// P52
                  LCD_PEN_PEN(1u << 21);    // P53

		// Cau hình các chân trên mat sau LCD
    LCD->BPEN[0] = LCD_BPEN_BPEN(1U << 18) | LCD_BPEN_BPEN(1U << 19);
    LCD->BPEN[1] = LCD_BPEN_BPEN(1U << 20) | LCD_BPEN_BPEN(1U << 8);

		// cai dat COMn waveforms  8-bit registers 	
		LCD->WF8B[40] = (1<<0)|(1<<4);	//COM0
		LCD->WF8B[52] = (1<<1)|(1<<5);	//COM1
		LCD->WF8B[19] = (1<<2)|(1<<6);	//COM2
		LCD->WF8B[18] = (1<<3)|(1<<7);	//COM3
	
		// Khoi tao ket thuc
		LCD->GCR &= ~LCD_GCR_PADSAFE_MASK; // tat che an toan de chay lcd
		LCD->GCR |= LCD_GCR_LCDEN_MASK; // enabling LCD
		
		return 0;
}

		// Mang decoder và các hang so de hien thi trên LCD
const volatile uint8_t ABC[16] = {0xEE,0x66,0xCC,0xEE,0x66,0xAA,0xAA,0xEE,0xEE,0xEE,0xEE,0x22,0x88,0x66,0x88,0x88}; 
const volatile uint8_t DEFG[16] = {0xBB,0x00,0x77,0x55,0xCC,0xDD,0xFF,0x00,0xFF,0xDD,0xEE,0xFF,0xBB,0x77,0xFF,0xEE}; 
const volatile uint8_t digit_ABC[4] = {11,38,8,17}; 
const volatile uint8_t digit_DEFG[4] = {10,53,7,37}; 

uint8_t LCD_Print(uint8_t digit, uint8_t number) //digit 0 - less significant, digit 3 - most significant
{
	if(digit>3 && number>15) return 1u; 
	else if(number>15) return 2u; 
	else if(digit>3) return 3u; 
	LCD->WF8B[digit_ABC[digit]] =ABC[number]; 
	LCD->WF8B[digit_DEFG[digit]]=DEFG[number];
	return 0u;
	
}

void LCD_Clear(void)
{
	for(int i=0;i<4;i++) 
	{
		LCD->WF8B[digit_ABC[i]]=0x0;
		LCD->WF8B[digit_DEFG[i]]=0x0;
	}
}

uint8_t LCD_Number(uint16_t value)
{
    // Kiem tra lon hon 9999 
    if (value > 9999) 
        return 1;

    // mang luu gia tri 
    uint8_t tenpowers[4] = {0};
    for (uint8_t i = 0; i < 4; i++)
    {
        tenpowers[i] = value % 10;
        value /= 10;
    }

 
    for (uint8_t i = 0; i < 4; i++)
    {
        LCD_Print(i, tenpowers[i]); // In tung so
    }

    return 0;
}



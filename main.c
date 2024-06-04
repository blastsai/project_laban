#include "MKL46Z4.h"      // Thu vien chinh cho vi dieu khien
#include "mag3110.h"      // Thu vien cho cam bien MAG3110
#include "lcd.h"  				// Thu vien cho man hinh LCD     
#include "system.h"  // Thu vien cho cac ham he thong
#include "string.h"
#include <stdio.h>
#include <math.h>
volatile int system_state = 1; // 0: dung, 1: hoat dong


int main() {
    MAG3110_vector vector;	
    
    // Khoi tao
	
		init_system();
		I2C_init();
    MAG_init();
		LCD_Init();
	
    // buffer luu tru  MAG3110
	
    uint8_t buffer[6];
		int16_t angle;
		int16_t x_min = 0x7FFF;
		int16_t y_min = 0x7FFF;
		int16_t x_max = -0x8000;
		int16_t y_max = -0x8000;
		
		// x y sau hieu chinh
		float x_correction, y_correction;
		int16_t x_offset,y_offset;
		float scale_x_y;
		uint16_t j = 0;
		
		// hieu chinh 
		while(j < 2000) {
		// Doc du lieu tu cam bien MAG3110
		MAG_read(buffer, &vector);
		if (vector.x < x_min)
				x_min = vector.x;
		if(vector.x > x_max)
				x_max = vector.x;
		if (vector.y < y_min)
				y_min = vector.y;
		if(vector.y > y_max)
				y_max = vector.y;
		delay(30000);
		j++;
	}
		// Tinh toan offset , scale
		x_offset = (x_max + x_min) / 2;
		y_offset = (y_max + y_min) / 2;
		scale_x_y = (float) (x_max - x_min) / (y_max - y_min);
		
    while (1) {
					if(system_state == 1){
						
					MAG_read(buffer, &vector);
					
				
					x_correction = (float) (vector.x - x_offset);
					y_correction = (float) (vector.y - y_offset) * scale_x_y;
			
					angle = (int16_t) (atan2(y_correction , x_correction) * 180 / 3.1415);
					if (angle < 0)
					angle = 360 + angle;
					LCD_Number(angle);
					delay(350000);
					}
					else{}
						
						
			}
}

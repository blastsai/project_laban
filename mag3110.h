#ifndef MAG3110_H
#define MAG3110_H

#include "MKL46Z4.h"


typedef struct MAG3110_vector {
	int16_t x, y, z;
} MAG3110_vector;


// Khai báo hàm
void I2C_init(void);
void MAG_init(void);
void MAG_read(uint8_t buffer[], MAG3110_vector* vector);
void MAG3110_write(uint8_t addr, uint8_t data);
uint8_t MAG3110_read(uint8_t address);
void delay(uint32_t us);
#endif // MAG3110_H


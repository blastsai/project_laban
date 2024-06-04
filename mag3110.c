#include "mag3110.h"

uint32_t i; // bien diem


// Che do mag
#define CTRL_REG1 0x10
#define CTRL_REG2 0x11
#define MAG3110_I2C_ADDRESS 0x0E
#define trangthai_dr  0x00


// Ðinh nghia các thanh ghi cua MAG3110.
#define x_msb 0x01
#define x_lsb 0x02
#define y_msb 0x03
#define y_lsb 0x04
#define z_msb 0x05
#define z_lsb 0x06



void delay(uint32_t us) {
		for(i = 0; i < us; i++);	
}

void I2C_write_byte(uint8_t data)
{
    I2C0->D = data;
}

uint8_t I2C_read_byte()
{
    return I2C0->D;
}


void I2C_init()   
{
    
		// Kích hoat clock cho module I2C0 va port E
		SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

		// Cau hình chân cho giao dien I2C0  cho MAG3110
		PORTE->PCR[24] = PORT_PCR_MUX(5);
		PORTE->PCR[25] = PORT_PCR_MUX(5);

		
		// Thiet lap baudrate và kích hoat I2C0.
		I2C0->F  = 0x1F; // baudrate
		I2C0->C1 = 0x80; // enable IIC
}


// Hàm doc 1 byte tu thanh ghi cua MAG3110 qua I2C.
uint8_t MAG3110_read(uint8_t addr)
{
    uint8_t result=0;
	
		// Thiet lap che do master và truyen
    I2C0->C1 |= I2C_C1_MST_MASK;;
    I2C0->C1 |= I2C_C1_TX_MASK;;;
	
		// Gui dia chi cua slave, bit R/W = 0 de ghi
    I2C_write_byte((MAG3110_I2C_ADDRESS << 1) | 0);

		
    // Cho ghi thanh cong
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;

		// Gui dia chi  thanh ghi can doc
    I2C_write_byte(addr);
    
		// Cho Mag phan hoi
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;
		
		// Thiet lap  start condition lap (SR)
    I2C0->C1 |= 0x04;;
		
		//Gui dia chi cua slave, bit R/W = 0 de doc tu thanh ghi 
    I2C_write_byte(MAG3110_I2C_ADDRESS << 1 | 1);
    
		// cho Mag phan hoi
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;;

		// truyen du lieu toi thanh ghi yeu cau
    I2C0->C1 &= ~I2C_C1_TX_MASK;;

		// Master không phan hoi (NAK), truyen du lieu thanh cong
    I2C0->C1 |= I2C_C1_TXAK_MASK;;
    result = I2C_read_byte();
    
		// Cho Mag phan hoi
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;;

		// Stop de ket thuc qua trinh truyen 
    I2C0->C1 &= ~I2C_C1_MST_MASK;;
    I2C0->C1 &= ~I2C_C1_TX_MASK;;;
		
    result = I2C_read_byte();
		
    // cho de tra ve ket qua
    delay(100);
    return result;
}

// Hàm ghi 1 byte vào thanh ghi cua  MAG3110 qua I2C.
void MAG3110_write(uint8_t addr, uint8_t data)
{
		// Thiet lap che do master và truyen
    I2C0->C1 |= I2C_C1_MST_MASK;;
    I2C0->C1 |= I2C_C1_TX_MASK;;;

		// Gui dia chi cua slave, bit R/W = 0 de ghi
    I2C_write_byte((MAG3110_I2C_ADDRESS << 1) | 0);
    
		// Cho Mag phan hoi
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;;

		// Gui dia chi  thanh ghi can doc
    I2C_write_byte(addr);
    
		// Cho Mag phan hoi
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;;

		// master truyen du lieu sau do ghi vao thanh ghi duoc chon
    I2C_write_byte(data);
    
		// Cho Mag phan hoi
		while((I2C0->S & I2C_S_IICIF_MASK)==0); 
		I2C0->S = I2C_S_IICIF_MASK;;;

		// Stop de dung qua trinh ghi
    I2C0->C1 &= ~I2C_C1_MST_MASK;;
    I2C0->C1 &= ~I2C_C1_TX_MASK;;;

    delay(100);
		
		
}

void MAG_init(void)
{
    // Ðat MAG3110 vào che do cho
		MAG3110_write(CTRL_REG1, 0x00);
    
		// thiet lap bit AUTO_MRST_EN trong CTRL_REG2.
		MAG3110_write(CTRL_REG2, 0x80);
	
		//  MAG3110 hoat dong ODR = 80 Hz và OSR = 1
    MAG3110_write(CTRL_REG1, 0x01);
}


void MAG_read(uint8_t buffer[], MAG3110_vector* vector)
{
			/* kiem tra 4 bit dau thanh ghi 0x00,
		 * 1trong các bit dó là 1, có nghia là có du lieu moi,
		 * bit 3: 1 - du lieu moi
		 * bit 2: 1 - .x
		 * bit 1: 1 -  y
		 * bit 0: 1 -  z
		 */
    if((MAG3110_read(0x00) & 0xf) != 0)
    {
      buffer[0] = MAG3110_read(x_msb);
      buffer[1] = MAG3110_read(x_lsb);

      buffer[2] = MAG3110_read(y_msb);
      buffer[3] = MAG3110_read(y_lsb);

      buffer[4] = MAG3110_read(z_msb);
      buffer[5] = MAG3110_read(z_lsb);
			
			// Ðat lai vector
			vector->x = 0;
			vector->y = 0;
			vector->z = 0;

			// x là uint16_t, và buffer[] là uint8_t,
      vector->x  = buffer[0] << 8;
      vector->x |= buffer[1];

      vector->y  = buffer[2] << 8;
      vector->y |= buffer[3];

      vector->z  = buffer[4] << 8;
      vector->z |= buffer[5];
    }
}


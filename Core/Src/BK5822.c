#include "BK5822.h"
//**************************
//mainly basic initialization + read/write registers
//**************************
uint8_t rx_buf[Rxbuf_size];
uint8_t rx_status;
uint8_t rx_status_d;
uint8_t rx_len;
uint8_t rx_err;

uint8_t rx_tmp;

uint8_t SwitchTx_USED;
uint8_t SwitchRx_USED;

//#define ver240401
#define verobu


#ifdef ver240401
//Attention: 
//This is NOT OBU version yet
//I havent start coding that
uint8_t init_reg0[] = {0x55, 0xA6, 0x58, 0x82};//RXLO=5.8
uint8_t init_reg1[] = {0x62, 0x7A, 0xBC, 0x40};//TXLO=5.835
uint8_t init_reg2[] = {0xB3, 0xFF, 0x42, 0xA0};//PLL
uint8_t init_reg3[] = {0x00, 0x03, 0x31, 0x00};//xtal
//uint8_t init_reg4[] = {0x2C, 0x5C, 0xEF, 0x3D};//tx random num, 0x2C = choose ceramic, 0x0C  = external
uint8_t init_reg4[] = {0x2C, 0x5C, 0xEF, 0x3F};//tx normal num
uint8_t init_reg5[] = {0x02, 0x2C, 0x75, 0xDC};
uint8_t init_reg6[] = {0x07, 0xAC, 0x08, 0xD9};
//uint8_t init_reg6[] = {0x04, 0xEC, 0x08, 0xD9};//changed recv 14k square to 3
uint8_t init_reg7[] = {0xD4, 0x21, 0x29, 0x87};
uint8_t init_reg8[] = {0x04, 0x08, 0x02, 0x15};

uint8_t init_reg9[] = {0x1D, 0x00, 0x41, 0x80};
uint8_t init_reg10[] = {0xFF, 0x01, 0x00, 0x00};
uint8_t init_reg11[] = {0x41, 0x30, 0x14, 0x04};
uint8_t init_reg12[] = {0x00, 0x3E, 0x80, 0x80};
uint8_t init_reg13[] = {0x20, 0x00, 0x02, 0x80};
uint8_t init_reg14[] = {0x80, 0x00, 0x08, 0x00};
//uint8_t init_reg15[] = {0xD7, 0x13, 0xEC, 0x00};//tx random num,tx=512,rx=256
uint8_t init_reg15[] = {0xD3, 0x37, 0xEC, 0x00};//tx normal num, 0x17=trasmit 14k square, 0x13 = normal,tx=512,rx=512
//uint8_t init_reg15[] = {0xD3, 0x23, 0xEC, 0x00};//tx normal num, 0x17=trasmit 14k square, 0x13 = normal,tx=256,rx=512
#endif

#ifdef verobu
//uint8_t init_reg0[] = {0x57, 0xF4, 0x78, 0x82};//RXLO=5.835
uint8_t init_reg0[] = { 0x55, 0xA6, 0x58, 0x82 };//RXLO=5.8, for exchanging test
//uint8_t init_reg1[] = {0x62, 0x2C, 0x9A, 0x40};//TXLO=5.79
uint8_t init_reg1[] = {0x62, 0x7A, 0xBC, 0x40};//TXLO=5.83, for exchanging test
uint8_t init_reg2[] = {0xB3, 0xFF, 0x42, 0xA0};//PLL
uint8_t init_reg3[] = {0x00, 0x03, 0x31, 0x00};//xtal
uint8_t init_reg4[] = {0x2C, 0x5C, 0xEF, 0x3F};//tx normal num
//uint8_t init_reg4[] = {0x2C, 0x1C, 0xEF, 0x3D};//tx carrier test
uint8_t init_reg5[] = {0x02, 0x2C, 0x75, 0xDC};
uint8_t init_reg6[] = {0x04, 0xF4, 0x48, 0xDA};
uint8_t init_reg7[] = {0xD4, 0x21, 0x29, 0x87};
uint8_t init_reg8[] = {0x04, 0x08, 0x02, 0x15};

//reg9-18:low byte->high byte
uint8_t init_reg9[] = {0x1D, 0x10, 0x04, 0x18};
uint8_t init_reg10[] = {0xFF, 0x01, 0x00, 0x00};
uint8_t init_reg11[] = {0x41, 0x30, 0x14, 0x04};
uint8_t init_reg12[] = {0x00, 0x3E, 0x00, 0x80};
uint8_t init_reg13[] = {0x20, 0x00, 0x02, 0x80};
uint8_t init_reg14[] = {0x80, 0x00, 0x08, 0x00};
uint8_t init_reg15[] = {0xD3, 0x33, 0x74, 0x07};//tx normal num,rx=512k,tx=512k, 0xC3 = CRC disable
//uint8_t init_reg15[] = {0xD3, 0x13, 0x74, 0x07};//tx normal num,rx=256k,tx=512k, 0xC3 = CRC disable

#endif

uint8_t init_reg17[] = {0x00,0x04,0x04,0x88,0x84,0x62,0xC1,0x68,0x38,0x7E,0xBE,0xE7,0xDF,0xFF};
uint8_t init_reg18[] = {
//0x08,0x84,0x81,0xA8,0xBC,0xFF,0xBB,0x8C,0x82,0x18,0x04
0x04,0x81,0x40,0x18,0x0A,0xF3,0x30,0x8A,0x41,0x08,0x01//77% ASK
};

//***************************
//Function defines
//***************************
void BK_read_reg(uint8_t reg, uint8_t *buf, uint8_t length)
{
	//reg = reg
	BKenable();
	HAL_SPI_Transmit(&hspi1, &reg, 1, 0xff);//BK5822 will return its status at the same time, but i dont got it
	HAL_SPI_Receive(&hspi1, buf, length, 0x1000);
	BKdisable();
}

void BK_write_reg(uint8_t reg, uint8_t *buf, uint8_t length)
{
	//reg = writereg + reg (needed for instructions)
	if(reg != writereg+reg16)
	{
	BKenable();
	HAL_SPI_Transmit(&hspi1, &reg, 1, 0xff);
	HAL_SPI_Transmit(&hspi1, buf, length, 0x1000);
	BKdisable();
	}
	else
	{
	BKenable();
	HAL_SPI_Transmit(&hspi1, &reg, 1, 0xff);
	for(int i=0;i<length;i++){
	HAL_SPI_Transmit(&hspi1, &buf[i], 1, 0x1000);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)0xff, 1, 0x1000);//Transmit some shit
	}
	BKdisable();
	}
}

//Intr might be needed, leave blank for intr
void BK_clear_intr(uint8_t intr_num)
{
	uint8_t intrbuff[4];
	BK_read_reg(reg15, intrbuff, 4);
	intrbuff[3] |= intr_num;
	BK_write_reg(writereg|reg15, intrbuff, 4);
}
//leave blank

void BK_write_fifo(uint8_t *buf, uint8_t length)
{
	uint8_t status[4];
	BK_read_reg(reg6, status, 4);
	if(!(status[0]&ASK_Tx_Full))
	{
		BK_write_reg(writereg+reg16, buf, length);
	}
	else printf("Error: Bk5822 has a full Tx buf!\n");
}

void BK_init(void)
{
	BK_write_reg((writereg|reg0), init_reg0, 4);
	BK_write_reg((writereg|reg1), init_reg1, 4);
	BK_write_reg((writereg|reg2), init_reg2, 4);
	BK_write_reg((writereg|reg3), init_reg3, 4);
	BK_write_reg((writereg|reg4), init_reg4, 4);
	BK_write_reg((writereg|reg5), init_reg5, 4);
	BK_write_reg((writereg|reg6), init_reg6, 4);
	BK_write_reg((writereg|reg7), init_reg7, 4);
	BK_write_reg((writereg|reg8), init_reg8, 4);
	
	BK_write_reg((writereg|reg9), init_reg9, 4);
	BK_write_reg((writereg|reg10), init_reg10, 4);
	BK_write_reg((writereg|reg11), init_reg11, 4);
	BK_write_reg((writereg|reg12), init_reg12, 4);
	BK_write_reg((writereg|reg13), init_reg13, 4);
	BK_write_reg((writereg|reg14), init_reg14, 4);
	BK_write_reg((writereg|reg15), init_reg15, 4);

	BK_write_reg((writereg+reg17), init_reg17, 14);
	BK_write_reg((writereg+reg18), init_reg18, 11);
	
	init_reg15[3] = 0x10;
	BK_write_reg((writereg|reg15), init_reg15, 4);
	HAL_Delay(2);
	init_reg15[3] = 0x00;
	BK_write_reg((writereg|reg15), init_reg15, 4);
	HAL_Delay(2);
#ifdef DEBUG_MODE
	printf("BK5822 init completed.\n");
#endif
}

//Read funtion : BK5822->IRQ triggers the IO intr on F439, start reading.
//Place this in GPIO Callback function. Then link the IO with BK5822->IRQ.
void BK_read_fifo(void)
{
	//rx_buf[], rx_status, rx_len
	uint8_t readbuff[4];
	BK_read_reg(reg6, readbuff, 4);
	rx_status = readbuff[0];
	if(rx_status & ASK_Irqn_Rx_Done)
	{
		//read reg5 byte1 for recved count in fifo
		BK_read_reg(reg5, readbuff, 4);
		rx_len = readbuff[0];
		#ifdef DEBUG_MODE
			printf("Recved %d bytes in FIFO!\n", rx_len);
		#endif
		for(int i=0;i<rx_len;i++){
			BK_read_reg(reg16, &rx_tmp, 1);
			rx_buf[i] = rx_tmp;
		}
		if(rx_len == 0) BK_clear_intr(ASK_Irqn_Rx_Done);
		rx_err = 0x00;
		//intr will be cleared automaticly after the reading above.
	}
	if(rx_status & ASK_Irqn_Tx_Done)
	{
		BK_clear_intr(ASK_Irqn_Tx_Done);
	}
	if(rx_status & ASK_Irqn_Rx_Err)
	{
		rx_err = 0x01;
		#ifdef DEBUG_MODE
			printf("Rx_Err occurred!\n");
		#endif
		BK_clear_intr(ASK_Irqn_Rx_Err);
	}
}

void BK_openrx(void)
{
	uint8_t readbuff[4];
	BK_read_reg(reg6, readbuff, 4);
	rx_status = readbuff[0];
	printf("rx_status = %02x\n", rx_status);
	//BK_clear_intr(ASK_Irqn_Rx_Err);
	if(rx_status == 0x5c) BK_clear_intr(ASK_Irqn_Rx_Err);
	else if(rx_status == 0x9a){ 
		BK_read_reg(reg5, readbuff, 4);
		rx_len = readbuff[0];
		printf("Recved %d bytes in FIFO!\n", rx_len);
		BK_read_reg(reg16, rx_buf, rx_len);
		for(int i=0;i<rx_len;i++){
			printf("%02x,", rx_buf[i]);
		}
		printf("\n");
	}
}


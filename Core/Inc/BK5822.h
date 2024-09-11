//*******************************************************************
//Attention!
//This header file is used together with :
//BK5822.c
//UART(redirected printf)
//SPI(4pin, master)
//GPIO(intr enabled)
//This header file contains:
//Basic includes
//reg defines
//status defines
//recv flag and recv buffer
//*******************************************************************

#include "spi.h"
#include "stdio.h"
#include "gpio.h"
#include "usart.h"

#define Rxbuf_size 128
#define DEBUG_MODE

#define FIFO_FULL 1
#define FIFO_EMPTY 0
#define readreg 0x00
#define writereg 0x20

#define reg0 0x00
#define reg1 0x01
#define reg2 0x02
#define reg3 0x03
#define reg4 0x04
#define reg5 0x05//read for buf count
#define reg6 0x06//read for fifo and intr status
#define reg7 0x07
#define reg8 0x08
#define reg9 0x09
#define reg10 0x0A
#define reg11 0x0B
#define reg12 0x0C
#define reg13 0x0D
#define reg14 0x0E
#define reg15 0x0F//bit 24-26 = intr ctrl
#define reg16 0x10
#define reg17 0x11
#define reg18 0x12

#define ASK_Rx_Full       (1<<7)
#define ASK_Rx_Empty      (1<<6)
#define ASK_Tx_Full       (1<<5)
#define ASK_Tx_Empty      (1<<4)
#define ASK__Reserved     (1<<3)
#define ASK_Irqn_Rx_Err  (1<<2)
#define ASK_Irqn_Rx_Done  (1<<1)
#define ASK_Irqn_Tx_Done  (1<<0)
#define ASK_Irqn_ALL (ASK_Irqn_Rx_Err|ASK_Irqn_Rx_Done|ASK_Irqn_Tx_Done)

#define BKenable() HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
#define BKdisable() HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);

extern uint8_t rx_buf[Rxbuf_size];
extern uint8_t rx_status;
extern uint8_t rx_status_d;
extern uint8_t rx_len;
extern uint8_t rx_tmp;
extern uint8_t SwitchTx_USED;
extern uint8_t SwitchRx_USED;
extern uint8_t rx_err;

//****************
//Function defines
//****************
void BK_init();
void BK_read_fifo();
void BK_write_fifo(uint8_t *buf, uint8_t length);
void BK_read_reg(uint8_t reg, uint8_t *buf, uint8_t length);
void BK_write_reg(uint8_t reg, uint8_t *buf, uint8_t length);
void BK_clear_intr(uint8_t intr_num);
void BK_openrx();

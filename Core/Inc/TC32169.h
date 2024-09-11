///////////////////////////////////////////////////////////
/*
This file contains driver and functional functions for TC32169

Pin usage:
SPI2, TC_IRQ_Pin, TC_CE_Pin, TC_TXRX_Pin
SPI2_NSS_Pin





*/
////////////////////////////////////////////////////////////
#ifndef __TC32169_H__
#define __TC32169_H__

#include "spi.h"
#include "cstdio"
#include "gpio.h"
#include "usart.h"

#define TCenable() HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
#define TCdisable() HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

#define TCwrite 0x20
#define TCread 0x00

#define _reg2 0x00
#define _reg3 0x01
#define _reg4 0x02
#define _reg5 0x04
#define _reg6 0x05
#define _reg7 0x06
#define _reg8 0x07
#define _reg9 0x08
#define _reg10 0x09
#define _reg11 0x0A
#define _reg12 0x0B
#define _reg13 0x10
#define _reg14 0x11
#define _reg15 0x12
#define _reg16 0x13
#define _reg17 0x14
#define _reg18 0x15
#define _reg19 0x16
#define _reg20 0x17
#define _reg21 0x18
#define _reg22 0x19
#define _reg23 0x1A
#define _reg24 0x1B
#define _reg25 0x1C
#define _reg26 0x1D
#define _reg27 0x2C
#define _reg28 0x36
#define _reg29 0x3C
#define _reg30 0x3D
#define _reg31 0x3E
#define _reg32 0x43
#define _reg33 0x56
#define _reg34 0x5C
#define _reg35 0x5D

#define D9 0x0200
#define D8 0x0100
#define D7 0x0080
#define D6 0x0040
#define D5 0x0020
#define D4 0x0010
#define D3 0x0008
#define D2 0x0004
#define D1 0x0002
#define D0 0x0001

extern uint8_t TC_buf[128];
extern uint8_t TC_intr[2];
extern uint8_t TC_txlen_err;
extern uint8_t TC_post_err;
extern uint8_t TC_rx_abort;
extern uint8_t TC_crc_ng;
extern uint8_t TC_fm0dec_err;
extern uint8_t TC_rxlen_err;
extern uint8_t TC_flag_err;
extern uint8_t TC_pre_err;
extern uint8_t TC_tx_done;
extern uint8_t TC_rx_ready;
extern volatile uint8_t TC_recv_stat;
extern volatile uint8_t TC_tran_stat;

void TC_Next_Rx();
void TC_Next_Tx();
void TC_TxPrepare();
void TC_clear_fifo();
void TC_clear_intr(uint8_t* intr_status);
void TC_debug_intr(uint8_t* status, uint8_t* txlen_err, uint8_t* post_err, uint8_t* rx_abort,
	uint8_t* crc_ng, uint8_t* fm0dec_err, uint8_t* rxlen_err, uint8_t* flag_err, uint8_t* pre_err,
	uint8_t* tx_done, uint8_t* rx_ready);
void TC_read_intr(uint8_t* intr_status);
void TC_read_length(uint8_t* length);
void TC_simple_debug_intr(uint16_t status_16);
void TC_soft_reset();
void TC_write_reg_special(uint8_t reg, uint8_t* buf);
void TC_read_reg(uint8_t reg, uint8_t* buf, uint8_t length);
void TC_write_reg(uint8_t reg, uint8_t* buf, uint8_t length);
void TC_read_fifo(uint8_t* buf, uint16_t length);
void TC_write_fifo(uint8_t* buf, uint8_t length);
void TC_init();
void TC_autoreset();

#endif
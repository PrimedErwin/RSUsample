///////////////////////////////////
/*
RSU version
need RX and TX, no sleep

BOOT SEQUENCE:
pwr on-> CE=0-> CE=1->
set TXRX-> set registers(PLL/RFfrequency,page31,33)->
loop begin{
tran/recv packs-> switch stat(page34,35)
}

ATTENTION:
- Before sending, write correspond freq to registers
- Read and write reg contains auto waiting for security purpose,
	replace them with Delay_us would be better
*/
//////////////////////////////////

#include "TC32169.h"
#include "stdint.h"
#include "tim.h"

uint8_t TC_buf[128];
uint8_t TC_intr[2];//It has a lot of intr, take care
uint8_t TC_txlen_err;
uint8_t TC_post_err;
uint8_t TC_rx_abort;
uint8_t TC_crc_ng;
uint8_t TC_fm0dec_err;
uint8_t TC_rxlen_err;
uint8_t TC_flag_err;
uint8_t TC_pre_err;
uint8_t TC_tx_done;
uint8_t TC_rx_ready;
volatile uint8_t TC_recv_stat;
volatile uint8_t TC_tran_stat;


#define version1

//Registers are indexed as datasheet, page 41
//MSB, I guess high byte first
//NL = Not linked, not used by TC_init();
#ifdef version1
static uint8_t init_reg2[] = {};//software reset, write by button intr, NL --TODO
static uint8_t init_reg3[] = {};//TXRX_en, NL
static uint8_t init_reg4[] = { 0x00, 0x00 };//DIO select, write to enable intr
static uint8_t init_reg5[] = {};
static uint8_t init_reg6[] = {};//PLL RX set, NL
static uint8_t init_reg7[] = {};
static uint8_t init_reg8[] = {};//PLL TX set, NL
static uint8_t init_reg9[] = {};//PLL reset, NL
static uint8_t init_reg10[] = { 0x00, 0x03 };//TX output power, page 87, 0003 = 0dBm
static uint8_t init_reg11[] = { 0x00, 0x06 };//Analog settings, page 87, modulation index = 0.8
static uint8_t init_reg12[] = {};//Wake-up sensitivity, NL
static uint8_t init_reg13[] = {};//FIFO, NL
static uint8_t init_reg14[] = {};//TC_read_intr, NL
static uint8_t init_reg15[] = {0x00, 0x0C};//Intr mask
static uint8_t init_reg16[] = {};//TC_clear_intr, NL
static uint8_t init_reg17[] = {};//Invert intr polarity, NL
static uint8_t init_reg18[] = { 0x00, 0x00 };//Bit rate, 1000 = tx256, rx512, 0000 = all512
static uint8_t init_reg19[] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1B, 0x12,
0x0E, 0x08, 0x07, 0x03, 0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x08,
0x0E,0x12,0x1B,0x1F };//ramp fifo, can be NL
//static uint8_t init_reg19[] = { 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
//0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
//};//ramp fifo, for OOK
static uint8_t init_reg20[] = {};//Max data recv length, NL
static uint8_t init_reg21[] = {};//Wake-up num, NL
static uint8_t init_reg22[] = {};//Wake-up work, NL
static uint8_t init_reg23[] = {};//Wake-up calibrate, NL
static uint8_t init_reg24[] = {};//Wake-up clear, NL
static uint8_t init_reg25[] = {};//Write data to wake-up reg, NL
static uint8_t init_reg26[] = {};//Rx data length, NL
static uint8_t init_reg27[] = { 0x00, 0x0C };//CRC setting, non-inv, MSB, CCITT, all1
static uint8_t init_reg28[] = { 0x00, 0x01 };//FIFO auto clear, 0001 = disable timer, can be NL
static uint8_t init_reg29[] = { 0x00, 0x02 };//Test, can be NL
static uint8_t init_reg30[] = {};//Wake-up freq, NL
static uint8_t init_reg31[] = {};//Write data to wake-up reg, NL
static uint8_t init_reg32[] = {};//Clear FIFO, NL
static uint8_t init_reg33[] = {};//OSC trim, NL
static uint8_t init_reg34[] = {};//Select CRC work, set to default, NL
static uint8_t init_reg35[] = {};//CRC data recv, NL
#endif // version1


/// <summary>
/// Functions
/// operations on registers and fifo
/// </summary>
/// <param name="reg">register names</param>
/// <param name="buf">buffer</param>
/// <param name="length">data length</param>
void TC_read_reg(uint8_t reg, uint8_t* buf, uint8_t length)
{
	TCenable();
	HAL_SPI_Transmit(&hspi2, &reg, 1, 0xffff);
	for (int i = 0; i < length; i += 2)
	{
		HAL_SPI_Receive(&hspi2, &buf[i], 2, 0xffff);
	}
	TCdisable();
	//HAL_Delay(1);
	//Or use this to skip 2 clk, but I never tried
	//HAL_SPI_Transmit(&hspi2, (uint8_t*)(0xffff), 2, 0x2);
	delay_us(5);
}

void TC_write_reg(uint8_t reg, uint8_t* buf, uint8_t length)
{
	TCenable();
	HAL_SPI_Transmit(&hspi2, &reg, 1, 0xffff);
	for (int i = 0; i < length; i += 2)
	{
		HAL_SPI_Transmit(&hspi2, &buf[i], 2, 0xffff);
	}
	TCdisable();
	//	HAL_Delay(1);
		//Or use this to skip 2 clk, but I never tried
	//HAL_SPI_Transmit(&hspi2, (uint8_t*)(0xffff), 2, 0x2);
	delay_us(5);
}
/// <summary>
/// for 32bytes reg write especially
/// </summary>
/// <param name="reg"></param>
/// <param name="buf"></param>
/// <param name="length"></param>
void TC_write_reg_special(uint8_t reg, uint8_t* buf)
{
	TCenable();
	HAL_SPI_Transmit(&hspi2, &reg, 1, 0xffff);
	HAL_SPI_Transmit(&hspi2, buf, 32, 0xffff);
	TCdisable();
	//	HAL_Delay(1);
		//Or use this to skip 2 clk, but I never tried
	//HAL_SPI_Transmit(&hspi2, (uint8_t*)(0xffff), 2, 0x2);
	delay_us(5);
}

/// <summary>
/// Attention, after software reset, reinit TC32169
/// </summary>
void TC_soft_reset()
{
	uint8_t reg = TCwrite | _reg2;
	uint8_t reset[2] = { 0x00,0x80 };

	TC_write_reg(reg, reset, 2);
}

void TC_read_length(uint8_t* length)
{
	uint8_t reg = TCread | _reg26;
	TC_read_reg(reg, length, 2);

}

void TC_read_fifo(uint8_t* buf, uint16_t length)
{
	uint8_t reg = TCread | 0x10;
	TCenable();
	HAL_SPI_Transmit(&hspi2, &reg, 1, 0xffff);
	HAL_SPI_Receive(&hspi2, buf, length, 0xffff);
	TCdisable();
}

void TC_write_fifo(uint8_t* buf, uint8_t length)
{
	uint8_t reg = TCwrite | 0x10;
	TCenable();
	HAL_SPI_Transmit(&hspi2, &reg, 1, 0xffff);
	HAL_SPI_Transmit(&hspi2, buf, length, 0xffff);
	TCdisable();
}

void TC_clear_fifo()
{
	uint8_t reg = TCwrite | _reg32;
	uint8_t clean[2] = { 0x01,0x00 };
	TC_write_reg(reg, clean, 2);
}

void TC_init()
{
	//set TXRX to low, preset
	TC_Rx();
	TCdisable();
	//HAL_Delay(3);
	delay_us(300);
	//set CE to enable chip
//	printf("dwadwaw\n");
	TC32169_BOOT();
	//Tx mode to default
	TC_Tx();
	//HAL_Delay(3);
	delay_us(44);

	//write registers needed
	TC_write_reg(TCwrite | _reg4, init_reg4, 2);
	TC_write_reg(TCwrite | _reg10, init_reg10, 2);
	TC_write_reg(TCwrite | _reg11, init_reg11, 2);
	TC_write_reg(TCwrite | _reg15, init_reg15, 2);
	TC_write_reg(TCwrite | _reg18, init_reg18, 2);
	TC_write_reg_special(TCwrite | _reg19, init_reg19);
	TC_write_reg(TCwrite | _reg27, init_reg27, 2);
	//TC_write_reg(TCwrite | _reg28, init_reg28, 2);
	//TC_write_reg(TCwrite | _reg29, init_reg29, 2);

}

void TC_read_intr(uint8_t* intr_status)
{
	uint8_t reg = TCread | _reg14;
	TC_read_reg(reg, intr_status, 2);
}

/// <summary>
/// This function will output debug info
/// </summary>
/// <param name="status_16"></param>
void TC_simple_debug_intr(uint16_t status_16)
{
	uint8_t txlen_err = (status_16 & D9) > 0 ? 1 : 0;
	uint8_t post_err = (status_16 & D8) > 0 ? 1 : 0;
	uint8_t rx_abort = (status_16 & D7) > 0 ? 1 : 0;
	uint8_t crc_ng = (status_16 & D6) > 0 ? 1 : 0;
	uint8_t fm0dec_err = (status_16 & D5) > 0 ? 1 : 0;
	uint8_t rxlen_err = (status_16 & D4) > 0 ? 1 : 0;
	uint8_t flag_err = (status_16 & D3) > 0 ? 1 : 0;
	uint8_t pre_err = (status_16 & D2) > 0 ? 1 : 0;
	uint8_t tx_done = (status_16 & D1) > 0 ? 1 : 0;
	uint8_t rx_ready = (status_16 & D0) > 0 ? 1 : 0;
	//if (txlen_err)
	//{
	//	printf("Invalid transmission length\n");
	//}
	//if (post_err)
	//{
	//	printf("Synchronization not detected\n");
	//}
	//if (rx_abort)
	//{
	//	printf("Abort sequence detected\n");
	//}
	//if (crc_ng)
	//{
	//	printf("CRC check error\n");
	//}
	//if (fm0dec_err)
	//{
	//	printf("FM0 decoding error\n");
	//}
	//if (rxlen_err)
	//{
	//	printf("Invalid receive length\n");
	//}
	//if (flag_err)
	//{
	//	printf("Frame start mark undetected\n");
	//}
	//if (pre_err)
	//{
	//	printf("Preamble undetected\n");
	//}
	//if (tx_done)
	//{
	//	printf("Transmission finished\n");
	//}
	if (rx_ready)
	{
		//printf("Receive finished\n");
		TC_recv_stat = 0x01;
	}
}
/// <summary>
/// 頼莎のデバッグ喘　ファンクション　です
/// </summary>
/// <param name="status"></param>
/// <param name="txlen_err"></param>
/// <param name="post_err"></param>
/// <param name="rx_abort"></param>
/// <param name="crc_ng"></param>
/// <param name="fm0dec_err"></param>
/// <param name="rxlen_err"></param>
/// <param name="flag_err"></param>
/// <param name="pre_err"></param>
/// <param name="tx_done"></param>
/// <param name="rx_ready"></param>
void TC_debug_intr(uint8_t* status, uint8_t* txlen_err, uint8_t* post_err, uint8_t* rx_abort,
	uint8_t* crc_ng, uint8_t* fm0dec_err, uint8_t* rxlen_err, uint8_t* flag_err, uint8_t* pre_err,
	uint8_t* tx_done, uint8_t* rx_ready)
{
	uint16_t status_16;
	status_16 = ((uint16_t)status[0] << 8) | status[1];
	*txlen_err = (status_16 & D9) > 0 ? 1 : 0;
	*post_err = (status_16 & D8) > 0 ? 1 : 0;
	*rx_abort = (status_16 & D7) > 0 ? 1 : 0;
	*crc_ng = (status_16 & D6) > 0 ? 1 : 0;
	*fm0dec_err = (status_16 & D5) > 0 ? 1 : 0;
	*rxlen_err = (status_16 & D4) > 0 ? 1 : 0;
	*flag_err = (status_16 & D3) > 0 ? 1 : 0;
	*pre_err = (status_16 & D2) > 0 ? 1 : 0;
	*tx_done = (status_16 & D1) > 0 ? 1 : 0;
	*rx_ready = (status_16 & D0) > 0 ? 1 : 0;

	//TC_simple_debug_intr(status_16);

	if (rx_ready)
	{
		//printf("Receive finished\n");
		TC_recv_stat = 0x01;
	}
	if (tx_done)
	{
		TC_tran_stat = 0x01;
	}
}

void TC_clear_intr(uint8_t* intr_status)
{
	uint8_t reg = TCwrite | _reg16;
	TC_write_reg(reg, intr_status, 2);
}
/// <summary>
/// Change TX frequency before sending packages
/// 000B0B26 is 5790MHz, 5835MHz needed
/// </summary>
void TC_TxPrepare()
{
	//page 33
	uint8_t ntx_msb = TCwrite | _reg8;
	uint8_t ntx_lsb = TCwrite | _reg7;
	uint8_t MSB[2] = { 0x00, 0x0B };
	uint8_t LSB[2] = { 0x21, 0x1F };

	uint8_t pllreset = TCwrite | _reg9;
	uint8_t pll[2] = { 0x00, 0x01 };
	TC_write_reg(ntx_msb, MSB, 2);
	TC_write_reg(ntx_lsb, LSB, 2);
	TC_write_reg(pllreset, pll, 2);
	delay_us(30);
	//HAL_Delay(3);

}
/// <summary>
/// set registers make its next state = Rx
/// 000B0B26 is 5790MHz
/// </summary>
void TC_Next_Rx()
{
	uint8_t nrx_msb = TCwrite | _reg6;
	uint8_t nrx_lsb = TCwrite | _reg5;
	uint8_t MSB[2] = { 0x00, 0x0B };
	uint8_t LSB[2] = { 0x0D, 0x97 };

	TC_write_reg(nrx_msb, MSB, 2);
	TC_write_reg(nrx_lsb, LSB, 2);
	//no PLL reset needed
}
/// <summary>
/// set registers make its next state = Tx
/// 000B211F is 5835MHz
/// </summary>
void TC_Next_Tx()
{
	uint8_t ntx_msb = TCwrite | _reg8;
	uint8_t ntx_lsb = TCwrite | _reg7;
	uint8_t MSB[2] = { 0x00, 0x0B };
	uint8_t LSB[2] = { 0x21, 0x1F };

	TC_write_reg(ntx_msb, MSB, 2);
	TC_write_reg(ntx_lsb, LSB, 2);
}

void TC_autoreset()
{
	TC_soft_reset();
	TC_write_reg(TCwrite | _reg4, init_reg4, 2);
	TC_write_reg(TCwrite | _reg10, init_reg10, 2);
	TC_write_reg(TCwrite | _reg11, init_reg11, 2);
	TC_write_reg(TCwrite | _reg15, init_reg15, 2);
	TC_write_reg(TCwrite | _reg18, init_reg18, 2);
	TC_write_reg_special(TCwrite | _reg19, init_reg19);
	TC_write_reg(TCwrite | _reg27, init_reg27, 2);

}
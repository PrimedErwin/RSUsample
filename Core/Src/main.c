/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  //俺は知ってる、このプロジェクトわ匂う、でもね
  //It walked through BK5822 which can really be called shit and TC32169
  //And hereby it's going to develop a real RSU for Shanxi transportation
  //Thinking that gonna not be possible but I still develop on this cuz gotta no way else
  //This project has been compiled into C/C++ mixed due to data structures
  //Which is pretty bad for coders
  //とにかく　これは本当に難しいですね
  //---by エルウィン はちがつ、にじゅうろくひ
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
//#include "string.h"
#include "cassert"
//#include "BK5822.h"
#include "frame.h"
#include "layerni.h"
#include "TC32169.h"
#include "gurenge.h"
#include "time.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//#define OBU_VERSION
//#define ACCURATE_RX_MODE
//#define RSU_VERSION_LEGACY

#define RSU_VERSION
//#define CHEAP_THRILLS//test without PC
#define INTEGRATE_F103//test with F103 on chip level
//#define TCtest
#define RTC_TEST

/***********全て必要************/
#define RSU_RECV_TEST
#define RSU_TRAN_TEST
//If u dont have a PC disable this
#define RSU_PC_INIT
/******************************/

#define transmit_MAXCNT 5

uint8_t Rxbuff;
volatile uint8_t FF_flg = 0;
uint8_t FF_cnt = 0;
uint16_t _frame_len = 0;
uint16_t _data_len = 0;
uint8_t pc_frame[MAX_LEN] = { 0 };
uint8_t pc_data[MAX_LEN] = { 0 };
uint8_t pc_data_tmp[MAX_LEN] = { 0 };
EF04Opt ef04;

uint8_t LSDU[128] = { 0 };
uint8_t LSDU_len;
uint8_t mac_address[4] = { 0 };
uint8_t mac_control;
uint8_t FCS[2];
uint8_t tx_buf[128];
uint8_t reg_test[4];
uint8_t rsctl = 0x00;

uint8_t RESET_triggered = 0;
uint8_t TC_rx_recved = 1;
volatile uint8_t transmit_permit = 0;
int rand_len;

extern uint8_t SwitchTx_USED;
extern uint8_t SwitchRx_USED;


#ifdef INTEGRATE_F103
volatile uint8_t F103_triggered = 0;
#endif // INTEGRATE_F103


#ifdef TCtest
uint8_t tctestarray[12] = { 0x7E, 0x88, 0x88, 0x88, 0x88, 0x07, 0x03, 0x91, 0x6D, 0x00, 0x02, 0x7E };
uint8_t tctestfreq[2] = { 0x00, 0x00 };
#endif // TCtest

#ifdef RSU_RECV_TEST
uint8_t tc_length[2];
uint16_t tc_length16;
#endif // TC_RECV_TEST


macctl_up OBU_macctl = LPDU_exist;
Macs OBU_mac = { 0xFF, 0xFF, 0xFF, 0xFF };//test mac
#ifdef OBU_VERSION
Commu_Stage OBU_stage = RSU_BST;
#endif

#ifdef RSU_VERSION
Commu_Stage RSU_stage = RSU_BST;
#endif

#ifdef RTC_TEST
uint8_t unix[4] = { 0x5c, 0x2c, 0x6f, 0xcf };//16:01:19
const unsigned int ucnt = _u8_4_to_32(unix);
volatile unsigned int ucnt2;
#endif // RTC_TEST

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//int fputc(int ch, FILE* fp)  // 自定义串口重定向函数，将printf输出到USART3便于调试
//{
//    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xff);
//    return ch;
//}
/* ------------------??????printf???????1?-------------------*/
namespace mystd
{
	template <int _Size = 128>
	void printf(const char* _Format)
	{
		char* words = new char[_Size];
		int len = sprintf(words, "%s", _Format);
		//assert(len < _Size);//corruption
		int cnt = 0;
		while (cnt < len)
		{
			HAL_UART_Transmit(&huart2, (uint8_t*)&words[cnt++], 1, 0xff);
		}
		delete[] words;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	//Assume uart? recved a frame from PC
	//Storage the whole frame into an array including 0xFF
	if (Rxbuff == PACK_START)
	{
		pc_frame[_frame_len++] = Rxbuff;
		FF_cnt++;
	}
	else
	{
		if (FF_cnt == 2) pc_frame[_frame_len++] = Rxbuff;
		//0xffff represents pack starts
	}
	if (FF_cnt == 3) {
		FF_cnt = 0;
		FF_flg = 1;
		//a frame ends
	}
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&Rxbuff, 1);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	transmit_permit = 1;
}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//uint8_t BK_ID_read(void);
//void BK_reg_all_read(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */
		//これで俺が説明する
		/******************************/
		/*
		* Startup
		* Init by PC(UART)
		* keep BSTing
		* Trade, communicate with PC
		* done
		*/
		/******************************/
	int transmit_cnt = 0;
	uint8_t reTranlen = 0;
	//srand(0x7FFFFFFF);

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CRC_Init();
	MX_SPI1_Init();
	MX_SPI2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_USART2_UART_Init();
	MX_RTC_Init();
	/* USER CODE BEGIN 2 */
	//printf("SYSTEM INIT\n");
	srand(2077);
	//srand(time(NULL));
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&Rxbuff, 1);
	//BK_init();
	TC_init();
	ToggleLD1();
	OBU_mac.macctl = (uint8_t)OBU_macctl;
	//rx_status_d = 0x00;
	SwitchRx();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	TC_TxPrepare();
	//printf("TC32169 has been initialized. TX mode by default.\n");
	//#ifdef RSU_RECV_TEST
//	TC_Next_Rx();
//	TC_Rx();
//	delay_us(44);
//	printf("MODE CHANGED TO RECV\n");
//#endif // RSU_RECV_TEST
#ifdef RSU_TRAN_TEST
	transmit_permit = 1;

#ifdef RSU_PC_INIT

	//Here init by PC upward

	//trans B0, 0x98
	uint8_t* B0temp = new uint8_t[27]{
		0xB0, 0x00, 0x01,
		0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
		0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
		0x77, 0x66, 0x12, 0x34, 0x56, 0x88, 0x88,
		0x00, 0x00, 0x00, 0x00, 0x00
	};
	frame_build(0x98, pc_frame, B0temp, (uint16_t)27U);
	HAL_UART_Transmit(&huart2, pc_frame, 27 + 5, 0xff);
	memset(pc_frame, 0, 128);
	//recv C0
	while (!FF_flg)
	{
	}
	frame_parse(pc_frame, pc_data, &rsctl, _frame_len, &_data_len);
	if (pc_data[0] != 0xC0)
	{
		mystd::printf<>("ERROR NO INIT FRAME RECEIVED\n");
		exit(EXIT_FAILURE);
	}
	unix[0] = pc_data[1];
	unix[1] = pc_data[2];
	unix[2] = pc_data[3];
	unix[3] = pc_data[4];
	ef04.EF04Opt = pc_data[17];
	ef04.EF04offset = (uint16_t)(pc_data[18] << 8 | pc_data[19]);
	ef04.EF04len = (uint16_t)(pc_data[20] << 8 | pc_data[21]);
	//trans B0, not 0x98
	memset(pc_frame, 0, 128);
	frame_build(0x78, pc_frame, B0temp, (uint16_t)27U);
	delete[] B0temp;
	HAL_UART_Transmit(&huart2, pc_frame, 27 + 5, 0xff);
	//recv empty
	while (!FF_flg)
	{
	}
	memset(pc_frame, 0, 128);

#endif // RSU_PC_INIT


#endif // RSU_TRAN_TEST

#ifdef RTC_TEST
	unixTime_setRTC(unix, &hrtc);
#endif // RTC_TEST

	uint8_t* BaconID = new uint8_t[4]{ 0xAA,0xBB,0xCC,0xDD };
	uint8_t* len22 = new uint8_t[2]{ 0x00, 0x01 };
	uint8_t* len122 = new uint8_t[2]{ 0x00, 0x01 };
	uint8_t* len152 = new uint8_t[2]{ 0x00, 0x01 };
	uint8_t* len192 = new uint8_t[2]{ 0x00, 0x01 };
	_BST cbst(BaconID, unix, 0x00, 0x1A,
		len22, len122, len152, len192);
	delete[] BaconID;
	delete[] len22;
	delete[] len122;
	delete[] len152;
	delete[] len192;

	uint8_t* RndRSU = new uint8_t[8]{ 0x12, 0x23, 0x34, 0x45,
	0x56, 0x67, 0x78, 0x89 };
	_GetSecureRq cgsrq(RndRSU, 0x90, 0x01);
	delete[] RndRSU;

	_TransferChannelRq ctcrq(0x99);

	_EventReportRq cerrq;

	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		for (int i = 0; i < 1e6 / 1000; i++)
		{
			delay_us(1000);
		}
		unixTime_setFrame(unix, &hrtc);
		ucnt2 = _u8_4_to_32(unix);
		if ((ucnt2 - ucnt) % 5 == 0) ToggleLD2();
		mystd::printf<15>("Debug session.\n");
		//HAL_UART_Transmit(&huart2, unix, 4, 0xff);
#ifdef TCtest
			//TC_TxPrepare();
	//		TC_read_reg(TCread | _reg7, tctestfreq, 2);
	//		printf("First=%02X%02X\n", tctestfreq[0], tctestfreq[1]);
		TC_write_fifo(tctestarray, 3);
		HAL_Delay(10);
		//		TC_read_reg(TCread | _reg10, tctestfreq, 2);
		//		printf("PWR=%02X%02X, reference = 0003\n", tctestfreq[0], tctestfreq[1]);
#endif // TCtest
#ifdef RSU_TRAN_TEST
		if (RSU_stage >= RSU_BST && RSU_stage <= RSU_release)
		{
			//F103_triggered = detected car in range
			if (F103_triggered || RSU_stage == RSU_release)
			{
				//初めてのジェネレーション
				if (transmit_cnt == 0)
				{
					memset(LSDU, 0, 128);
					memset(tx_buf, 0, 128);
					//tx_buf[0] = (uint8_t)RSU_stage;
					//rand_len = rand() % 10;
					//for (int i = 1; i <= rand_len; i++)
					//{
					//	tx_buf[i] = tx_buf[i] | (uint8_t)rand();
					//	if (tx_buf[i] == 0x7E) tx_buf[i] = 0x7F;
					//}

					//LSDU_build(LSDU, tx_buf, rand_len + 1);
					//memset(tx_buf, 0, 128);
					//recv_build(tx_buf, LSDU, &OBU_mac, rand_len + 3);
					//Transmit at the beginning
					//TC_write_fifo(tx_buf, 3 + 9 + rand_len);
					if (RSU_stage == RSU_BST)
					{
						OBU_mac.macadd[0] = 0xFF;
						OBU_mac.macadd[1] = 0xFF;
						OBU_mac.macadd[2] = 0xFF;
						OBU_mac.macadd[3] = 0xFF;
						unixTime_setFrame(unix, &hrtc);
						cbst.update_unix(unix);
						cbst.msg_construct(tx_buf);
						LSDU_build(LSDU, tx_buf, cbst.lengthwa());
						recv_build(tx_buf, LSDU, &OBU_mac, cbst.lengthwa() + 2);
						TC_write_fifo(tx_buf, cbst.lengthwa() + 11);
						reTranlen = cbst.lengthwa() + 11;
					}
					else if (RSU_stage == RSU_baseinfo)
					{
						cgsrq.msg_construct(tx_buf);
						LSDU_build(LSDU, tx_buf, cgsrq.lengthwa());
						recv_build(tx_buf, LSDU, &OBU_mac, cgsrq.lengthwa() + 2);
						TC_write_fifo(tx_buf, cgsrq.lengthwa() + 11);
						reTranlen = cgsrq.lengthwa() + 11;
					}
					else if (RSU_stage == RSU_release)
					{
						cerrq.msg_construct(tx_buf);
						LSDU_build(LSDU, tx_buf, cerrq.lengthwa());
						recv_build(tx_buf, LSDU, &OBU_mac, cerrq.lengthwa() + 2);
						TC_write_fifo(tx_buf, cerrq.lengthwa() + 11);
						reTranlen = cerrq.lengthwa() + 11;
					}
					else//ctcrq //am i writing ptaisa lol
					{

					}
					transmit_cnt++;
				}
				//transmit if didnt recv anything
				// a timer is needed for every 13 ms permits a transmission
				//plan - TIM4, 13000us cycle
				if (transmit_permit && !TC_rx_recved)
				{
					TC_Next_Tx();
					TC_Tx();
					delay_us(44);
					//printf("Didn't recv, retrasmitting...\n");
					TC_write_fifo(tx_buf, reTranlen);
					transmit_cnt++;
					transmit_permit = 0;
					//exceeded
					if (transmit_cnt > transmit_MAXCNT)
					{
						RSU_stage = RSU_BST;
						transmit_cnt = 0;
						//printf("Congratulations! Transmission Stack Overload!\n");
					}
				}
				//transmitted, wait for recv->change stat or retrans
				if (TC_tran_stat == 0x01)
				{
					TC_rx_recved = 0;
					TC_tran_stat = 0x00;
					//next for RX
					TC_Next_Rx();
					TC_Rx();
					delay_us(44);

				}
			}
		}
#endif // RSU_TRAN_TEST

#ifdef RSU_RECV_TEST
		if (TC_recv_stat == 0x01)
		{
			//got frame
			  //printf("Received the following msgs: \n");
			TC_recv_stat = 0x00;
			TC_read_length(tc_length);
			tc_length16 = ((uint16_t)tc_length[0] << 8) | tc_length[1];
			//printf("Length = %d\n", tc_length16);
			if (/*tc_length16 == 0 || */TC_rxlen_err || TC_rx_abort)
			{
				RESET_triggered = 1;
				TC_autoreset();
				TC_Next_Rx();
				TC_Rx();
				delay_us(44);
			}
			if (!RESET_triggered && tc_length16)
			{
				TC_read_fifo(TC_buf, tc_length16);
				for (int i = 0; i < tc_length16; i++)
				{
					printf("%02X, ", TC_buf[i]);
				}
				printf("\n");
				//parse frame
				recv_parse(TC_buf, mac_address, &mac_control, LSDU, FCS, &LSDU_len);
				//state machine switch
				if (RSU_stage == RSU_BST)
				{
					//recved VST, info store
					OBU_mac.macadd[0] = mac_address[0];
					OBU_mac.macadd[1] = mac_address[1];
					OBU_mac.macadd[2] = mac_address[2];
					OBU_mac.macadd[3] = mac_address[3];
					pc_data[0] = 0xB2;
					pc_data[1] = mac_address[0];
					pc_data[2] = mac_address[1];
					pc_data[3] = mac_address[2];
					pc_data[4] = mac_address[3];
					pc_data[5] = 0x00;
					pc_data[6] = LSDU[9];
					pc_data[7] = LSDU[10];
					pc_data[8] = LSDU[11];
					pc_data[9] = LSDU[12];
					pc_data[10] = LSDU[13];
					pc_data[11] = LSDU[14];
					pc_data[12] = LSDU[15];
					pc_data[13] = LSDU[16];//contractProvider
					pc_data[14] = LSDU[17];//contractType
					pc_data[15] = LSDU[18];//contractVer
					pc_data[16] = LSDU[19];
					pc_data[17] = LSDU[20];
					pc_data[18] = LSDU[21];
					pc_data[19] = LSDU[22];
					pc_data[20] = LSDU[23];
					pc_data[21] = LSDU[24];
					pc_data[22] = LSDU[25];
					pc_data[23] = LSDU[26];//contractSerialNum
					pc_data[24] = LSDU[27];
					pc_data[25] = LSDU[28];
					pc_data[26] = LSDU[29];
					pc_data[27] = LSDU[30];
					pc_data[28] = LSDU[31];
					pc_data[29] = LSDU[32];
					pc_data[30] = LSDU[33];
					pc_data[31] = LSDU[34];//contractDate
					pc_data[32] = 0x00;
					pc_data[33] = LSDU[LSDU_len - 2];
					pc_data[34] = LSDU[LSDU_len - 1];
					//transmit B2 to PC
					data_preprocess(pc_data_tmp, pc_data, 35, &_data_len, 't');
					rsctl = 0x08;
					frame_build(rsctl, pc_frame, pc_data_tmp, _data_len);
					HAL_UART_Transmit(&huart2, pc_frame, _data_len + 5, 0xfff);
					//nothing to do, wait for PC's C1, state->baseinfo there
				}
				else if (RSU_stage == RSU_baseinfo)
				{
					//recved baseinfo of vehicle, info process

					//change to state 00190002 to new dsrc

				}
				else if (RSU_stage == RSU_00190002)
				{
					//recved 00190002 of vehicle, 

					//change to state EF04

				}
				else if (RSU_stage == RSU_EF04)
				{
					//recved EF04

					//transmit B4 to PC

					//wait for PC's C6, state->EF04update
				}
				else if (RSU_stage == RSU_EF04update)
				{
					//recved EF04update

					//change to state 0019
				}
				else if (RSU_stage == RSU_0019)
				{
					//recved 0019

					//change to state 0002

				}
				else if (RSU_stage == RSU_0002)
				{
					//recved 0002

					//transmit B5 to PC

					//wait for PC's C1 and transmit EventReport
				}
				//release state ends in CHEAP THRILL

				/*
				if (LSDU[2] == (uint8_t)OBU_VST) {
					// transmit frame to PC
					printf("VST state set.\nOBU info:\n");
					for (int i = 3; i < LSDU_len; i++)
					{
						printf("%02X, ", LSDU[i]);
					}
					printf("\n");
#ifdef CHEAP_THRILLS
						RSU_stage = RSU_baseinfo;
						transmit_cnt = 0;
#endif // CHEAP_THRILLS
					}
					else if (LSDU[2] == (uint8_t)OBU_baseinfo) {
						//RSU stage to next, 00190002
						RSU_stage = RSU_00190002;
						transmit_cnt = 0;
					}
					else if (LSDU[2] == (uint8_t)OBU_00190002) {
						//RSU stage to next
						RSU_stage = RSU_EF04;
						transmit_cnt = 0;
					}
					else if (LSDU[2] == (uint8_t)OBU_EF04) {
						//transmit frame to PC
						printf("EF04 file of OBU has been recved.\n");
#ifdef CHEAP_THRILLS
						RSU_stage = RSU_EF04update;
						transmit_cnt = 0;
#endif // CHEAP_THRILLS
					}
					else if (LSDU[2] == (uint8_t)OBU_EF04update) {
						//RSU stage to next
						RSU_stage = RSU_0019;
						transmit_cnt = 0;
					}
					else if (LSDU[2] == (uint8_t)OBU_0019) {
						//RSU stage to next
						RSU_stage = RSU_0002;
						transmit_cnt = 0;
					}
					else if (LSDU[2] == (uint8_t)OBU_0002) {
						//transmit frame to PC
						printf("0002 file of OBU has been recved.\n");
#ifdef CHEAP_THRILLS
						RSU_stage = RSU_release;
						F103_triggered = 0;
						transmit_cnt = 0;
#endif // CHEAP_THRILLS
					}
					//SPECIALLY TEST FOR STAGE 6D
					else if (LSDU[2] == (uint8_t)0x6D) {
						//transmit frame to PC
						printf("6D file has been recved.\n");
#ifdef CHEAP_THRILLS
						RSU_stage = RSU_release;
						F103_triggered = 0;
						transmit_cnt = 0;
#endif // CHEAP_THRILLS
					}

					else {
						//RSU stage to next
						RSU_stage = RSU_BST;
						transmit_cnt = 0;
					}
					*/
				TC_rx_recved = 1;
				//next for TX
				TC_Next_Tx();
				TC_Tx();
				delay_us(44);
				//printf("MODE CHANGED TO TRAN\n");
			}
			RESET_triggered = 0;
			}
#ifndef CHEAP_THRILLS
		if (FF_flg) {
			printf("recved frame from pc\n");
			FF_flg = 0;
			//_frame_len pc_frame
			frame_parse(pc_frame, pc_data, &rsctl, _frame_len, &_data_len);
			_frame_len = 0;
			//Then get to know what to do next, for example transmit msg to OBU
			//also, if rx_status_d = 1, may need to transmit frame to PC.
			if (pc_data[0] == 0xC1 && RSU_stage == RSU_BST) {
				RSU_stage = RSU_baseinfo;
				transmit_cnt = 0;
			}
			else if (pc_data[0] == 0xC6 && RSU_stage == RSU_EF04) {
				RSU_stage = RSU_EF04update;
				transmit_cnt = 0;
			}
			else if (pc_data[0] == 0xC1 && RSU_stage == RSU_0002) {
				RSU_stage = RSU_release;
				transmit_cnt = 0;
			}
		}
#endif // !CHEAP_THRILLS

#endif // RSU_RECV_TEST

		//BK_reg_all_read();
		//parsing for frames from pc or 5822
#ifdef OBU_VERSION

#ifdef ACCURATE_RX_MODE
	  //rx_len rx_buf
	  //parse the frame, then 5822 write fifo to response
	  //or uart transmit to pc..

#endif
#endif

#ifdef RSU_VERSION_LEGACY
		//RSU to lane controller
		if (RSU_stage >= RSU_BST && RSU_stage <= RSU_release) {
			//transmit BST to initialize trading
			SwitchTx();
			if (transmit_cnt < transmit_MAXCNT) {
				transmit_cnt++;
				memset(LSDU, 0, Rxbuf_size);
				tx_buf[0] = (uint8_t)RSU_stage;
				rand_len = rand() % 10;
				for (int i = 1; i <= rand_len; i++) {
					tx_buf[i] = tx_buf[i] | (uint8_t)rand();
					if (tx_buf[i] == 0x7E) tx_buf[i] = 0x7F;
					//					printf("%02X-", tx_buf[i]);
				}
				LSDU_build(LSDU, tx_buf, 1 + rand_len);
				memset(tx_buf, 0, 256);
				recv_build(tx_buf, LSDU, &OBU_mac, 3 + rand_len);
				BK_write_fifo(tx_buf, 3 + 9 + rand_len);
			}
			else {
				transmit_cnt = 0;
				//printf("OBU no response\n");
				printf("RSU_stage = %02X, %d", (uint8_t)RSU_stage, RSU_stage);
				//RSU_stage = RSU_BST;
				RSU_stage += 1;
			}
		}
		if (FF_flg) {
			printf("recved frame from pc\n");
			FF_flg = 0;
			//_frame_len pc_frame
			frame_parse(pc_frame, pc_data, &rsctl, _frame_len, &_data_len);
			_frame_len = 0;
			//Then get to know what to do next, for example transmit msg to OBU
			//also, if rx_status_d = 1, may need to transmit frame to PC.
			if (rsctl == 0xC1 && RSU_stage == RSU_BST) {
				RSU_stage = RSU_baseinfo;
				transmit_cnt = 0;
			}
			else if (rsctl == 0xC6 && RSU_stage == RSU_EF04) {
				RSU_stage = RSU_EF04update;
				transmit_cnt = 0;
			}
			else if (rsctl == 0xC1 && RSU_stage == RSU_0002) {
				RSU_stage = RSU_release;
				transmit_cnt = 0;
			}
		}
		if (rx_status_d == 0x01) {
			//recved from OBU
			printf("recved msg, start processing\n");
			ToggleLD1();
			rx_status_d = 0x00;
			BK_read_fifo();
#ifdef DEBUG_MODE
			printf("rx_len = %02x\n", rx_len);
			for (int i = 0; i < rx_len; i++) {
				printf("%02x,", rx_buf[i]);
			}
#endif
			if (rx_len > 0) {
				//recv_parse(rx_buf, mac_address, &mac_control, LSDU, FCS, &LSDU_len);
				//LSDU contains command era and status era for 2 bytes. Exclude them.
				if (LSDU[2] == (uint8_t)OBU_VST) {
					// transmit frame to PC
					printf("Frame to PC : B2\n");
				}
				else if (LSDU[2] == (uint8_t)OBU_baseinfo) {
					//RSU stage to next, 00190002
					RSU_stage = RSU_00190002;
					transmit_cnt = 0;
		}
				else if (LSDU[2] == (uint8_t)OBU_00190002) {
					//RSU stage to next
					RSU_stage = RSU_EF04;
					transmit_cnt = 0;
				}
				else if (LSDU[2] == (uint8_t)OBU_EF04) {
					//transmit frame to PC
					printf("Frame to PC : B4\n");
				}
				else if (LSDU[2] == (uint8_t)OBU_EF04update) {
					//RSU stage to next
					RSU_stage = RSU_0019;
					transmit_cnt = 0;
				}
				else if (LSDU[2] == (uint8_t)OBU_0019) {
					//RSU stage to next
					RSU_stage = RSU_0002;
					transmit_cnt = 0;
				}
				else if (LSDU[2] == (uint8_t)OBU_0002) {
					//transmit frame to PC
					printf("Frame to PC : B5\n");
				}
				else {
					//RSU stage to next
					RSU_stage = RSU_BST;
					transmit_cnt = 0;
				}
	}
}
		if (RSU_stage == RSU_release) {
			RSU_stage = RSU_BST;
			transmit_cnt = 0;
		}
		//BK_clear_intr(ASK_Irqn_ALL);
#endif
		}

	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
//uint8_t BK_ID_read(void)
//{
//	uint8_t buffer[4];
//	uint8_t cmd[] = {0x00};//read reg0 7:0 for chipID
//	BKenable();
//	HAL_SPI_Transmit(&hspi1, cmd, 1, 0xff);
//	if(HAL_SPI_Receive(&hspi1, (uint8_t *)&buffer, 4, 0xff) != HAL_OK)
//	{
//		printf("ID read ERROR\n");
//		return 0xFF;
//	}
//	BKdisable();
//	printf("reg0 = %02x%02x%02x%02x\n", buffer[0],buffer[1],buffer[2],buffer[3]);
//	//printf("chipID = %x\n", buffer[3]);
//	return buffer[0];
//}

/*void SPI1_self_test(void)
{
	uint8_t cmd[] = {'S','P','I','-','-','1'};
	HAL_SPI_TransmitReceive(&hspi1, cmd, (uint8_t *)&buffer, 6, 0xff);
	printf("SPI1 selftesting...\nThe following line will show SPI--1 if it's ok\n");
	printf("%s\n", buffer);
	printf("If the line beyond looks strange pls check SPI1 MOSI-MISO connectivity\n");
}
*/
//void BK_reg_all_read(void)
//{
//	ToggleLD1();
//	BK_read_reg(reg0, reg_test, 4);
//		printf("reg0 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg0, reg_test, 4);
//		printf("reg0 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg1, reg_test, 4);
//		printf("reg1 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg2, reg_test, 4);
//		printf("reg2 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg3, reg_test, 4);
//		printf("reg3 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg4, reg_test, 4);
//		printf("reg4 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg5, reg_test, 4);
//		printf("reg5 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg6, reg_test, 4);
//		printf("reg6 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg7, reg_test, 4);
//		printf("reg7 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg8, reg_test, 4);
//		printf("reg8 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg9, reg_test, 4);
//		printf("reg9 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//	
//		BK_read_reg(reg10, reg_test, 4);
//		printf("reg10 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//	
//		BK_read_reg(reg11, reg_test, 4);
//		printf("reg11 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//	
//		BK_read_reg(reg12, reg_test, 4);
//		printf("reg12 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//	
//		BK_read_reg(reg13, reg_test, 4);
//		printf("reg13 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg14, reg_test, 4);
//		printf("reg14 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//
//		BK_read_reg(reg15, reg_test, 4);
//		printf("reg15 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg17, reg_test, 4);
//		printf("reg17 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg18, reg_test, 4);
//		printf("reg18 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		BK_read_reg(reg0, reg_test, 4);
//		printf("reg0 = %02x%02x%02x%02x\n", reg_test[0],reg_test[1],reg_test[2],reg_test[3]);
//		
//		printf("chipID = %02x\n", BK_ID_read());
//		//HAL_Delay(1500);
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	  /* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	  /* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
		 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

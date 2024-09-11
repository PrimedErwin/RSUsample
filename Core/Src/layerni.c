/*
This file contains obu testing stuff
Use without PSAM/ESAM, no TDES
Works for transaction between OBU and RSU
Including:
	MAC address
	MAC control
	LSDU (or LPDU but I dont know what's inside)
	CRC (FCS)

Attention:
	Not sure what will be received, assume that 0x7E is head and tail for every frame
*/

#include "layerni.h"

//#define print_frame

uint8_t lane_dir;//1=OBU->RSU, 0=RSU->OBU
uint8_t LSDU_exist;// 1=yes 0=no
uint8_t CR;//0 = command, 1 = response
uint8_t private_lane;//MAC address = 0xff then this is valid, 1=looking for lane, 0=not
const uint16_t crc_table[256] ={
0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};
	
//unsigned int min(unsigned num1, unsigned num2){
//	return num1>num2?num2:num1;
//}

//unsigned int strlength(uint8_t *str){
//	unsigned int len = 0, zeros = 0;
//	for(int i=0; i<256; i++){
//		len++;
//		if(str[i] == '\n'){
//			return len-1;
//		}
//		else if(str[i] == 0x00){
//			zeros++;
//			if(zeros >= min(5, 256-i-1) && str[i+1] == 0x00){
//				return len-zeros;
//			}
//		}
//		else zeros = 0;
//	}
//	return 0;
//}
////Calculate strlen cuz strlen func stops at 0x00 which really cofused me
////Make sure (better) the last byte indexes MAX_LEN-5 or more!! cuz length below this will work unstable

//uint8_t *FastStrcat(uint8_t *pszDest, uint8_t* pszSrc, uint8_t len)
//{
//    /*
//    while(*pszDest)
//        pszDest++;
//    while((*pszDest++ = *pszSrc++));
//    return --pszDest;
//    */
//    pszDest += (uint8_t)strlength(pszDest);
////    unsigned int i = strlength(pszSrc);
//		unsigned int i = (unsigned int)len;
//    while(i--){
//        *pszDest++ = *pszSrc++;
//    }
//    return --pszDest;
//}
//cat strings cuz strcat func also stops at 0x00
//cant cat if last byte is 0x00(which means info is 0x00 in the last byte)

uint16_t crc_16(uint8_t *data, uint8_t len)
{
	//CRC16-CCITT
    uint16_t crc16 = 0xFFFF;
    uint16_t crc_h8, crc_l8;
    
    while( len-- ) {
        crc_h8 = (crc16 >> 8);
        crc_l8 = (crc16 << 8);
        crc16 = crc_l8 ^ crc_table[crc_h8 ^ *data];
        data++;
    }

    return crc16;
}

void recv_parse(uint8_t* frame, uint8_t* mac_address, uint8_t* mac_control, uint8_t* LSDU, uint8_t* FCS, uint8_t* LSDU_len)
{
	//assume that recved a frame from RSU (*frame)
	uint8_t start = 0;
	int i = 0;
	while (frame[i]) {
#ifdef print_frame
		printf("%02X, ", frame[i]);
#endif
		if (frame[i] == 0x7E) {
			start = i + 1;
			break;
		}
		i++;
	}
	for (i = 0; i < 4; i++) {
#ifdef print_frame
		printf("%02X, ", frame[start + i]);
#endif
		mac_address[i] = frame[start + i];
	}
	start += 4;
#ifdef print_frame
	printf("%02X, ", frame[start]);
#endif
	* mac_control = frame[start++];
	//start now is the first bit of LSDU
	i = 0;
	while (frame[start + i] != 0x7E) {
		LSDU[i] = frame[start + i];
#ifdef print_frame
		printf("%02X, ", frame[start + i]);
#endif
		i++;
		if (i == 256) break;
	}
#ifdef print_frame
	printf("%02X\n", frame[start + i]);
#endif
	* LSDU_len = (uint8_t)i - 2;
	FCS[1] = LSDU[--i];
	LSDU[i] = 0x00;
	FCS[0] = LSDU[--i];
	LSDU[i] = 0x00;
	//now we have mac info and LSDU and FCS
}

void recv_mac_control(uint8_t mac_control)
{
	lane_dir = mac_control & 0x80;
	LSDU_exist = mac_control & 0x40;
	CR = mac_control & 0x20;
	private_lane = mac_control & 0x10;
}

void recv_build(uint8_t *frame, uint8_t *LSDU, Macs *mac, uint8_t LSDU_len)
{
	//frame = ready to be transmitted
	//LSDU = LSDU
	//mac = Macs mac
	unsigned int pos = 0;
	uint16_t FCS = 0;
	frame[pos++] = FRAME_START;
	frame[pos++] = mac->macadd[0];
	frame[pos++] = mac->macadd[1];
	frame[pos++] = mac->macadd[2];
	frame[pos++] = mac->macadd[3];
	frame[pos++] = mac->macctl;
	FastStrcat(frame, LSDU, LSDU_len);
	pos += LSDU_len;
	FCS = crc_16(frame + 1, pos - 1);
	frame[pos++] = ((FCS & 0xFF00)>>8);
	frame[pos++] = FCS & 0xFF;
	frame[pos++] = FRAME_END;
}

void LSDU_build(uint8_t *LSDU, uint8_t *Msg, uint8_t Msg_len)
{
	//LSDU contains control era and info era
	//LSDU = LSDU
	//control era has type1 and type3, type3 need ACn, so I choose type1
	//then control era becomes 000P0011, and all P is 0, control era is 0x03
	unsigned int pos = 0;
	LSDU[pos++] = 0x03;
	LSDU[pos++] = 0x91;//no string cat
	//idk what the following should be, operations to OBE-SAM perhaps? 
	for(int i=0; i<Msg_len; i++){
		LSDU[pos+i] = Msg[i];
		//printf("MSG LOAD = %02X\n", Msg[i]);
	}
}

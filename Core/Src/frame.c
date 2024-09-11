/*
This file contains frame related stuff.
Works for transaction between RSU and PC(controller)
Including:
	Frame building
	Frame parsing
	BCC
	Data building(just put all the data together)
	
Attention:
	frame length can be calculated with 0xff head
	strlength will be wrong if data really has 5 more 0x00
*/
#include "frame.h"

/*
********************BASIC FUNCTIONS***************************
*/

//unsigned int min(unsigned num1, unsigned num2){
//	return num1>num2?num2:num1;
//}

//unsigned int strlength(uint8_t *str){
//	unsigned int len = 0, zeros = 0;
//	for(int i=0; i<MAX_LEN; i++){
//		len++;
//		if(str[i] == '\n'){
//			return len-1;
//		}
//		else if(str[i] == 0x00){
//			zeros++;
//			if(zeros >= min(5, MAX_LEN-i-1) && str[i+1] == 0x00){
//				return len-zeros;
//			}
//		}
//		else zeros = 0;
//	}
//	return 0;
//}
////Calculate strlen cuz strlen func stops at 0x00 which really cofused me
////Make sure (better) the last byte indexes MAX_LEN-5 or more!! cuz length below this will work unstable

//uint8_t *FastStrcat(uint8_t *pszDest, uint8_t* pszSrc)
//{
//    /*
//    while(*pszDest)
//        pszDest++;
//    while((*pszDest++ = *pszSrc++));
//    return --pszDest;
//    */
//    pszDest += (uint8_t)strlength(pszDest);
//    unsigned int i = strlength(pszSrc);
//    while(i--){
//        *pszDest++ = *pszSrc++;
//    }
//    return --pszDest;
//}
//cat strings cuz strcat func also stops at 0x00
//cant cat if last byte is 0x00(which means info is 0x00 in the last byte)

/*
********************FRAME BUILD/PARSE***************************
*/

void frame_build(uint8_t rsctl, uint8_t *frame, uint8_t *data, uint16_t data_len)
{
	//frame = builded frame
	//data = what u want to transmit
	unsigned int pos = 0;
	uint8_t BCC = 0;
	
	frame[pos++] = PACK_START;
	frame[pos++] = PACK_START;
	//0xffff = STX, start of frame
	
	frame[pos++] = rsctl;
	//0xX8 = rsctl, frame index
	BCC ^= rsctl;
	for(int i=0; i<data_len; i++) BCC ^= data[i];
	
	FastStrcat(frame, data, data_len);
	pos += data_len;
	//data
	
	frame[pos++] = BCC;
	//BCC
	
	frame[pos++] = PACK_TAIL;
	//frame end
	
}

void frame_parse(uint8_t *frame, uint8_t *data, uint8_t *rsctl, uint16_t frame_len, uint16_t *data_len)
{
	//frame = frame recved
	//data = what pc transmitted to u
	uint8_t data_start = 3;
	*rsctl = frame[2];
	uint8_t BCC = 0;
	BCC ^= *rsctl;
	
	if(frame[4] == 0xff)
	{
		data[0] = 0;
		return;//empty response
	}
	else if(frame[0] != 0xff || frame[1] != 0xff || frame[frame_len-1] != 0xff)
	{
		data[0] = 0;
		return;//not a frame
	}
	memcpy(data, frame+data_start, frame_len-data_start-2);
	*data_len = frame_len - 5;
	for(int i=0; i<frame_len-data_start-2; i++) BCC ^= data[i];
	
	if(BCC != frame[frame_len-2])
	{
		printf("Frame BCC check error!\n");
	}
}
/*
********************0xFF/0xFE PROCESS***************************
*/
//Process special byte 0xff in data
//transmit(build frame) 0xff = 0xfe 0x01, 0xfe = 0xfe 0x00
//receive(parse frame) 0xfe 0x01 = 0xff, 0xfe 0x00 = 0xfe

void data_preprocess(uint8_t *data, uint8_t *datawating, uint16_t data_len, uint16_t *len, char dir)
{
	//t = transmit
	//r = recv
	//datawating = before process
	//data_len = datawating len
	//data = after process
	//len = data len
	unsigned int posr = 0;
	unsigned int post = 0;
	if(dir == 't')
	{
        for(int i=0; i< data_len; i++){
            if(datawating[i] == 0xfe){
                data[post++] = 0xfe;
                data[post++] = 0x00;
            }
            else if(datawating[i] == 0xff){
                data[post++] = 0xfe;
                data[post++] = 0x01;
            }
            else{
                data[post++] = datawating[i];
            }
        }
				*len = post;
	}
	else if(dir == 'r')
	{
		for(int i=0; i<data_len; i++){
			if(datawating[i] == 0xfe){
				if(datawating[i+1] == 0x00){
					data[posr++] = 0xfe;
				}
				else if(datawating[i+1] == 0x01){
					data[posr++] = 0xff;
				}
				i++;
			}
			else{
				data[posr++] = datawating[i];
			}
		}
		*len = posr;
	}
}

/*
********************INITIAL DIFFERENT FRAME PARTS***************************
*/
//Next initial different frames from RSU to PC(controller) which CMDtype is 0xBX
//Assemble data -> data preprocess('t') -> frame build -> uart(RS232 to PC)
void data_build(uint8_t *data, uint16_t *data_len){
	unsigned int pos = 0;
	data[pos++] = 0xee;
	data[pos++] = 0xee;
	*data_len = pos;
}


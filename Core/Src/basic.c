#include "basic.h"
//#include <stm32f4xx_hal_rtc.h>

unsigned int min(unsigned num1, unsigned num2) {
	return num1 > num2 ? num2 : num1;
}

unsigned int strlength(uint8_t* str) {
	unsigned int len = 0, zeros = 0;
	for (int i = 0; i < 256; i++) {
		len++;
		if (str[i] == '\n') {
			return len - 1;
		}
		else if (str[i] == 0x00) {
			zeros++;
			if (zeros >= min(5, 256 - i - 1) && str[i + 1] == 0x00) {
				return len - zeros;
			}
		}
		else zeros = 0;
	}
	return 0;
}
//Calculate strlen cuz strlen func stops at 0x00 which really cofused me
//Make sure (better) the last byte indexes MAX_LEN-5 or more!! cuz length below this will work unstable

uint8_t* FastStrcat(uint8_t* pszDest, uint8_t* pszSrc, uint8_t len)
{
	/*
	while(*pszDest)
		pszDest++;
	while((*pszDest++ = *pszSrc++));
	return --pszDest;
	*/
	pszDest += (uint8_t)strlength(pszDest);
	//    unsigned int i = strlength(pszSrc);
	unsigned int i = (unsigned int)len;
	while (i--) {
		*pszDest++ = *pszSrc++;
	}
	return --pszDest;
}
//cat strings cuz strcat func also stops at 0x00
//cat len from pszSrc


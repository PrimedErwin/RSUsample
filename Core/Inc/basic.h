/*
This file contains basic functions for frame cat, len cal
*/

#include "stdint.h"
#include "string.h"
#include "cstdio"
#include "time.h"


unsigned int min(unsigned num1, unsigned num2);
uint8_t *FastStrcat(uint8_t *pszDest, uint8_t* pszSrc, uint8_t len);
unsigned int strlength(uint8_t *str);

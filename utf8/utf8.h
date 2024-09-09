#ifndef UNICODE_UTF8_H
#define UNICODE_UTF8_H

#include "rune.h"

#include <stdint.h>

#include "utils/iByteTrain_forw.h"

int Unicode_fromUtf8(const iByteTrain *train, Unicode_rune *nDestination);
#define Unicode_fromUtf8_EOF -1
#define Unicode_fromUtf8_TRAINERROR -2
#define Unicode_fromUtf8_ERROR 1
#define Unicode_fromUtf8_PREMATUREEOF 2
#define Unicode_fromUtf8_INVALID 3
#define Unicode_fromUtf8_MIDDLE 4

int Unicode_toUtf8(Unicode_rune rune, uint8_t *nDestination);
//#define Unicode_toUtf8_ERROR -1
#define Unicode_toUtf8_TOOBIG -2
/*
	expects destination to be 4 bytes long
	returns length of utf8 sequence on success
	returns negative value on error
*/

#endif

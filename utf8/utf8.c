
#include "./utf8.h"

#include <stddef.h>

#include "utils/iByteTrain.h"

#define UTF8x2_MIN 0x80
#define UTF8x3_MIN 0x0800
#define UTF8x4_MIN 0x010000
#define UTF8x4_MAX 0x10ffff
	//artificially capped

int Unicode_fromUtf8(const iByteTrain *train, Unicode_rune *nDestination) {

	uint8_t byte = 0;
	switch (iByteTrain_chewchew(train, &byte)) {
		case 0: break;
		case IBYTETRAIN_ENDOFTHELINE: return Unicode_fromUtf8_EOF;
		default: return Unicode_fromUtf8_TRAINERROR;
	}

	if (byte < UTF8x2_MIN) {
		if (nDestination != NULL) *nDestination = byte;
		return 0;
	}

	if (~byte & 0x40) {
		//middle of sequence
		return Unicode_fromUtf8_MIDDLE;
	}

	int8_t length = 2;
	for (uint8_t pos = 0x20; (byte & pos) && (pos != 0); pos = pos >> 1) length++;
	if (length > 4) {
		return Unicode_fromUtf8_ERROR;
	}

	Unicode_rune r = byte & (0xff >> length + 1);
	for (int8_t i = length - 1; i > 0; i--) {
		switch (iByteTrain_lookoutAhead(train, &byte)) {
			case 0: break;
			case IBYTETRAIN_ENDOFTHELINE: return Unicode_fromUtf8_PREMATUREEOF;
			default: return Unicode_fromUtf8_TRAINERROR;
		}

		if ((byte < UTF8x2_MIN) || (byte & 0x40)) {
			return Unicode_fromUtf8_INVALID;
		}
		iByteTrain_chewchew(train, NULL);
		
		r = (r << 6) | (byte & 0x3f);
	}

	if (nDestination != NULL) *nDestination = r;
	return 0;
}

int Unicode_toUtf8(Unicode_rune rune, uint8_t *nDestination) {

	if (rune < UTF8x2_MIN) {
		if (nDestination != NULL) nDestination[0] = rune;
		return 1;
	}

	if (rune < UTF8x3_MIN) {
		if (nDestination != NULL) {
			nDestination[1] = 0x80 | (rune & 0x3f);
			nDestination[0] = 0xc0 | (rune >> 6);
		}
		return 2;
	}

	if (rune < UTF8x4_MIN) {
		if (nDestination != NULL) {
			nDestination[2] = 0x80 | (rune & 0x3f);
			nDestination[1] = 0x80 | ((rune >> 6) & 0x3f);
			nDestination[0] = 0xe0 | (rune >> 12);
		}
		return 3;
	}

	if (rune <= UTF8x4_MAX) {
		if (nDestination != NULL) {
			nDestination[3] = 0x80 | (rune & 0x3f);
			nDestination[2] = 0x80 | ((rune >> 6) & 0x3f);
			nDestination[1] = 0x80 | ((rune >> 12) & 0x3f);
			nDestination[0] = 0xf0 | (rune >> 18);
		}
		return 4;
	}

	return Unicode_toUtf8_TOOBIG;
}

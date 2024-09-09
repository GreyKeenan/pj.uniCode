
#include "utf8/utf8.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "rune.h"

#include "utils/iByteTrain.h"

struct Reader {
	const char *s;
	uint32_t p;
};
int Reader_nextChar(void *vself, uint8_t *nDestination) {
	struct Reader *self = vself;
	char c = self->s[self->p];
	if (c == '\0') {
		return IBYTETRAIN_ENDOFTHELINE;
	}
	self->p++;
	if (nDestination != NULL) *nDestination = c;
	return 0;
}
int Reader_peekChar(const void *vself, uint8_t *nDestination) {
	const struct Reader *self = vself;
	char c = self->s[self->p];
	if (c == '\0') {
		return IBYTETRAIN_ENDOFTHELINE;
	}
	if (nDestination != NULL) *nDestination = c;
	return 0;
}
struct iByteTrain Reader_asIByteTrain(struct Reader *self) {
	return (iByteTrain) {
		.vself = self,
		.chewchew = &Reader_nextChar,
		.lookoutAhead = &Reader_peekChar
	};
}

bool compare(const void *va, const void *vb) {
	const uint8_t *a = va;
	const uint8_t *b = vb;
	for (int i = 0; i < 4; ++i) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}
void printUtf8Rune(const void *v) {
	const uint8_t *s = v;
	printf("[0x%x_0x%x_0x%x_0x%x](%s)",
		s[0], s[1], s[2], s[3], s
	);
}
int test_toUtf8(Unicode_rune r, int exE, const char *exStr) {
	int e = 0;
	uint8_t utf8Rune[5] = {0};
	e = Unicode_toUtf8(r, utf8Rune);

	printf("want (%d, ", exE);
	printUtf8Rune(exStr);
	printf(") from (0x%x) -> (%d, ", r, e);
	printUtf8Rune(utf8Rune);
	printf(")");

	if (e == exE && compare(exStr, utf8Rune)) {
		printf("\n");
		return 0;
	}
	printf("\t\t!\n");
	return 1;
}
int test_fromUtf8(const char *s, int exE, Unicode_rune exR) {

	struct Reader rdr = { .s = s };

	struct iByteTrain train = Reader_asIByteTrain(&rdr);

	Unicode_rune r = 0;
	int e = Unicode_fromUtf8(&train, &r);
	
	printf("want (%d, 0x%x) from (", exE, exR);
	printUtf8Rune(s);
	printf(") -> (%d, 0x%x)", e, r);

	if (e == exE && r == exR) {
		printf("\n");
		return 0;
	}

	printf("\t\t!\n");
	return 1;
}

int main(const int argc, const char **argv) {
	printf("\n: Testing ...\n");

	int pass = 0;

	printf("\ntesting toUtf8():\n");

	pass += test_toUtf8(0x1f0a0, 4, "🂠");
	pass += test_toUtf8(0x01000000, Unicode_toUtf8_TOOBIG, "\0\0\0\0");
	pass += test_toUtf8(0xffffffff, Unicode_toUtf8_TOOBIG, "\0\0\0\0");
	pass += test_toUtf8(0x1f0e3, 4, "🃣");

	printf("\ntoUtf8() tests %s!\n", pass? "failed":"passed");
	//great coverage. Next!
	
	printf("\ntesting fromUtf8():\n");
	pass = 0;

	pass += test_fromUtf8("¿", 0, 191);
	pass += test_fromUtf8("⁋", 0, 8267);
	pass += test_fromUtf8("🂠", 0, 0x1f0a0);
	
	//basically no coverage lmao. Im tired of writing tests though, and this is a personal proj, so who cares? Have fun, future me.

	printf("\nfromUtf8() tests %s!\n", pass? "failed":"passed");

	printf("\n: ... testing concluded.\n");
	return 0;
}

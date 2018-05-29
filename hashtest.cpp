#include "hashtest.h"
#include <math.h>


int main() {

	uint32_t output[HWUNITS][16];
	uint8_t msgColl;
	md4_hasher(output, msgColl);

}

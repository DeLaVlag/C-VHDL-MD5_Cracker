#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HWUNITS 2

void md4_hasher(uint32_t output[HWUNITS][16],uint8_t msgColl);

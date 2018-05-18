#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void md4_hasher(uint32_t output[16], uint32_t[4], uint8_t msgColl);
void md4_printer();
uint32_t md4_compress(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x[16], uint8_t index);
int test();

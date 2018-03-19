#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void md5_hasher(void);
void md5_hasher(uint32_t *o0,uint32_t *o1,uint32_t *o2,uint32_t *o3,
		uint32_t *o4,uint32_t *o5,uint32_t *o6,uint32_t *o7,uint32_t *o8,
		uint32_t *o9,uint32_t *o10,uint32_t *o11,uint32_t *o12,
		uint32_t *o13,uint32_t *o14,uint32_t *o15);
//void md5_hasher(uint8_t *input_r, uint32_t *out0, uint32_t *out1, uint32_t *out2, uint32_t *out3);
void md5_printer();
//void md5_printer(uint32_t *out0,uint32_t *out1,uint32_t *out2,uint32_t *out3);

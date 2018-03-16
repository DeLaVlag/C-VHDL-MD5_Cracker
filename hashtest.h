#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void md5_hasher(uint8_t *input_r);
//void md5_hasher(uint8_t *input_r, uint32_t *out0, uint32_t *out1, uint32_t *out2, uint32_t *out3);
void md5_printer(uint32_t *out0,uint32_t *out1,uint32_t *out2,uint32_t *out3);

#include "main.h"

//uint32_t h0,h1,h2,h3;
static int rnd_seed=1;

//The compress functions
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))


/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//Selectors for the functions
#define FF 0b00
#define GG 0b01
#define HH 0b10

//Gives the amount of positions it needs to rotate
static uint32_t r[] = {3, 7, 11, 19, 3, 7, 11, 19, 3, 7, 11, 19, 3, 7, 11, 19,
                3,  5, 9, 13, 3,  5, 9, 13, 3,  5, 9, 13, 3,  5, 9, 13,
                3, 9, 11, 15, 3, 9, 11, 15, 3, 9, 11, 15, 3, 9, 11, 15};



void md4_compress(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d, uint32_t x[16], uint8_t index){
#pragma HLS inline
	/**
	 * Determines which function we have to execute, as the functions change every 16 iterations, we only need to check 2 bits to determine which function to execute
	 * because:
	 * 			0b000000 = 0
	 * 			0b010000 = 16
	 * 			0b100000 = 32
	 * 			0b110000 = 48
	 * So we check the 5th nd 6th bit do determine the function
	 */
	uint8_t t_functionSelector = (index >> 4)&0b11;
	//Holds the values of the function
	uint32_t t_function = 0;
	uint8_t t_number = 0;
	switch(t_functionSelector){
	case FF:
		t_function = F(b,c,d);
		t_number = index;
		break;
	case GG:
		t_function = G(b,c,d) + 0x5a827999;
		t_number = ((index * 4)+((index>>2)&0b11) )% 16;
		break;
	case HH:
		t_function = H(b,c,d) + 0x6ed9eba1;
		t_number = (index*8) % 16 ;
		t_number += (((index>>1)&0b1)*4) % 16;
		t_number += (((index>>2)&0b1)*2) % 16;
		t_number += ((index>>3)&0b1) % 16;
		break;
	default:
		break;
	}
//#pragma HLS RESOURCE variable=t_function latency=2
	t_function += a + x[t_number];
	uint8_t rotate = r[index];
	a =  ROTATE_LEFT(t_function,rotate);

	//Switch the inputs around a= d, b =a, c= b, d= c
	uint32_t tmp = d;
	d = c;
	c = b;
	b = a;
	a = tmp;

}

void md4_hasher(uint32_t x[HWUNITS][16], uint8_t msgColl){

#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE s_axilite port=x
#pragma HLS INTERFACE s_axilite port=msgColl
//#pragma HLS PIPELINE

//	uint32_t x[16];
	uint32_t h0[HWUNITS],h1[HWUNITS],h2[HWUNITS],h3[HWUNITS];

	//init all with same IVs
	for(int i=0;i<HWUNITS;i++){
		h0[i]=0x67452301;
		h1[i]=0xefcdab89;
		h2[i]=0x98badcfe;
		h3[i]=0x10325476;
//		printf("IV: %08x\n",h3[i]);
	}

	//IHVS0
//	h0 = 0x67452301;
//	h1 = 0xefcdab89;
//	h2 = 0x98badcfe;
//	h3 = 0x10325476;

	//reversed for ez comparison MD5
//	uint32_t hash2Crack0 = 0xf40d733f;
//	uint32_t hash2Crack1 = 0x91c22ebc;
//	uint32_t hash2Crack2 = 0xd9e5d6a0;
//	uint32_t hash2Crack3 = 0x1185a773;

	//reversed for ez comparison MD5
	uint32_t hash2Crack0 = 0x255a70a9;
	uint32_t hash2Crack1 = 0xd5ea4439;
	uint32_t hash2Crack2 = 0x3582ef71;
	uint32_t hash2Crack3 = 0x2fa03819;

//	uint8_t msg2Hash[64] = "I just need a message that's 64B long such to hash 512b";

//	uint32_t a = h0, b = h1, c = h2, d = h3;//, x[16];

	hwUnits:for(uint8_t hw=0;hw<HWUNITS;hw++){
#pragma HLS unroll

		uint32_t a = h0[hw], b = h1[hw], c = h2[hw], d = h3[hw];

		for (uint8_t i=0;i<14;i++){
			int randval = rand_int();
			x[hw][i]=randval;
//			printf("%08x ",x[i]);
		}

		//setting the last part of msg with 1 and sizeof, with reversal
		x[hw][13]&=~(0xff000000);
		x[hw][13]|=(1<<31);
		x[hw][14]=0x1B8;
		x[hw][15]=0;

		compressLoop:for(int i = 0; i < 48; i++){
#pragma HLS PIPELINE
			md4_compress(a, b, c, d, x[hw], i);
		}

		h0[hw] += a;
		h1[hw] += b;
		h2[hw] += c;
		h3[hw] += d;

		//reversing done by software
		h0[hw] = ((h0[hw] & 0xFF)<<3*8) | ((h0[hw] & 0xFF00) << 8) | ((h0[hw] & 0xFF0000) >> 8) | ((h0[hw] & 0xFF000000) >> 3*8);
		h1[hw] = ((h1[hw] & 0xFF)<<3*8) | ((h1[hw] & 0xFF00) << 8) | ((h1[hw] & 0xFF0000) >> 8) | ((h1[hw] & 0xFF000000) >> 3*8);
		h2[hw] = ((h2[hw] & 0xFF)<<3*8) | ((h2[hw] & 0xFF00) << 8) | ((h2[hw] & 0xFF0000) >> 8) | ((h2[hw] & 0xFF000000) >> 3*8);
		h3[hw] = ((h3[hw] & 0xFF)<<3*8) | ((h3[hw] & 0xFF00) << 8) | ((h3[hw] & 0xFF0000) >> 8) | ((h3[hw] & 0xFF000000) >> 3*8);

		printf("\nhash=%08x%08x%08x%08x\n",h0[hw],h1[hw],h2[hw],h3[hw]);
//
//		//reversed is original message!
		printf("\n reversed message2:\n");
		//everything upto the 0x80 is hashed. check online for hexhasher.
		for (uint8_t i=0;i<16;i++){
			printf("%08x",((x[hw][i] & 0xFF)<<3*8) | ((x[hw][i] & 0xFF00) << 8) | ((x[hw][i] & 0xFF0000) >> 8) | ((x[hw][i] & 0xFF000000) >> 3*8));
		}

		//write outputs when hash is found
		if (h0[hw]==hash2Crack0&&h1[hw]==hash2Crack2&&h2[hw]==hash2Crack2&&h3[hw]==hash2Crack3){
//			msgColl=hw+1;
			msgColl=1;
		}
	}
}

uint32_t rand_int (void)
{
    unsigned int hi,lo;

    hi = 16807 * (rnd_seed >> 16);
    lo = 16807 * (rnd_seed & 0xFFFF);
    lo += (hi & 0x7FFF) << 16;
    lo += hi >> 15;
    if (lo > 2147483647)
        lo -= 2147483647;
    rnd_seed = lo;
    return rnd_seed;
}


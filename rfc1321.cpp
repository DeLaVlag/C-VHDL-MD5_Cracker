#include "main.h"

//uint32_t h0,h1,h2,h3;
//uint32_t h0[HWUNITS],h1[HWUNITS],h2[HWUNITS],h3[HWUNITS];
static int rnd_seed=1;

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF 0b00
#define GG 0b01
#define HH 0b10
#define II 0b11

static uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

//Use binary fractional part of the sines of integers as constants:
static uint32_t k[] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};


void md5_compress(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d, uint32_t x[16], uint8_t index){
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
	//Determines which part of the message we use in the function
	uint8_t t_number = 0;
	//Holds the values of the function
	uint32_t t_function = 0;
	switch(t_functionSelector){
	case FF:
		t_function = F(b,c,d);
		t_number = index;
		break;
	case GG:
		t_function = G(b,c,d);
		t_number = (1 + 5*index) %16;
		break;
	case HH:
		t_function = H(b,c,d);
		t_number = (5 + 3*index) %16;
		break;
	case II:
		t_function = I(b,c,d);
		t_number = (7*index) %16;
		break;
	}
	t_function += a + x[t_number] + k[index];
	t_function = ROTATE_LEFT(t_function,r[index]);

	//Switch the inputs around a= d, b =a, c= b, d= c
	uint32_t tmp = d;
	d = c;
	c = b;
	b += t_function;
	a = tmp;

}

void md5_hasher(uint32_t x[16], uint8_t msgC){

#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE s_axilite port=x
#pragma HLS INTERFACE s_axilite port=msgC
//#pragma HLS PIPELINE

//	uint32_t h0,h1,h2,h3;
	uint32_t h0[HWUNITS],h1[HWUNITS],h2[HWUNITS],h3[HWUNITS];
//#pragma HLS resource variable=h0 core=RAM_2P_BRAM
//#pragma HLS resource variable=h1 core=RAM_2P_BRAM
//#pragma HLS resource variable=h2 core=RAM_2P_BRAM
//#pragma HLS resource variable=h3 core=RAM_2P_BRAM

	//init all with same IVs
	for(int i=0;i<HWUNITS;i++){
		h0[i]=0x67452301;
		h1[i]=0xefcdab89;
		h2[i]=0x98badcfe;
		h3[i]=0x10325476;
//		printf("IV: %08x\n",h3[i]);
	}
//uint32_t h0,h1,h2,h3;
	//IHVS0
//	 h0 = 0x67452301;
//	 h1 = 0xefcdab89;
//	 h2 = 0x98badcfe;
//	 h3 = 0x10325476;

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

//		uint32_t a = h0, b = h1, c = h2, d = h3;
		uint32_t a = h0[hw], b = h1[hw], c = h2[hw], d = h3[hw];

		for (uint8_t i=0;i<14;i++){
			x[i] = rand_int(h0[hw-1]+h1[hw-1]+h2[hw-1]+h3[hw-1]);
//			printf("%08x ",x[i]);
		}

		//setting the last part of msg with 1 and sizeof, with reversal
		x[13]&=~(0xff000000);
		x[13]|=(1<<31);
		x[14]=0x1B8;
		x[15]=0;

		compressLoop:for(int i = 0; i < 64; i++){
#pragma HLS PIPELINE
			md5_compress(a, b, c, d, x, i);
		}

//		h0 += a;
//		h1 += b;
//		h2 += c;
//		h3 += d;

		h0[hw] += a;
		h1[hw] += b;
		h2[hw] += c;
		h3[hw] += d;

		//reversing done by software
//		h0 = ((h0 & 0xFF)<<3*8) | ((h0 & 0xFF00) << 8) | ((h0 & 0xFF0000) >> 8) | ((h0 & 0xFF000000) >> 3*8);
//		h1 = ((h1 & 0xFF)<<3*8) | ((h1 & 0xFF00) << 8) | ((h1 & 0xFF0000) >> 8) | ((h1 & 0xFF000000) >> 3*8);
//		h2 = ((h2 & 0xFF)<<3*8) | ((h2 & 0xFF00) << 8) | ((h2 & 0xFF0000) >> 8) | ((h2 & 0xFF000000) >> 3*8);
//		h3 = ((h3 & 0xFF)<<3*8) | ((h3 & 0xFF00) << 8) | ((h3 & 0xFF0000) >> 8) | ((h3 & 0xFF000000) >> 3*8);

		h0[hw] = ((h0[hw] & 0xFF)<<3*8) | ((h0[hw] & 0xFF00) << 8) | ((h0[hw] & 0xFF0000) >> 8) | ((h0[hw] & 0xFF000000) >> 3*8);
		h1[hw] = ((h1[hw] & 0xFF)<<3*8) | ((h1[hw] & 0xFF00) << 8) | ((h1[hw] & 0xFF0000) >> 8) | ((h1[hw] & 0xFF000000) >> 3*8);
		h2[hw] = ((h2[hw] & 0xFF)<<3*8) | ((h2[hw] & 0xFF00) << 8) | ((h2[hw] & 0xFF0000) >> 8) | ((h2[hw] & 0xFF000000) >> 3*8);
		h3[hw] = ((h3[hw] & 0xFF)<<3*8) | ((h3[hw] & 0xFF00) << 8) | ((h3[hw] & 0xFF0000) >> 8) | ((h3[hw] & 0xFF000000) >> 3*8);

//		printf("\nhash=%08x%08x%08x%08x\n",h0,h1,h2,h3);

		printf("\nhash=%08x%08x%08x%08x\n",h0[hw],h1[hw],h2[hw],h3[hw]);
////
////		//reversed is original message!
//		printf("\n reversed message2:\n");
//		//everything upto the 0x80 is hashed. check online for hexhasher.
		for (uint8_t i=0;i<16;i++){
			printf("%08x",((x[i] & 0xFF)<<3*8) | ((x[i] & 0xFF00) << 8) | ((x[i] & 0xFF0000) >> 8) | ((x[i] & 0xFF000000) >> 3*8));
		}

		//write outputs when hash is found
//		if (h0==hash2Crack0&&h1==hash2Crack2&&h2==hash2Crack2&&h3==hash2Crack3){
//			msgC=1;
//		}

		if (h0[hw]==hash2Crack0&&h1[hw]==hash2Crack2&&h2[hw]==hash2Crack2&&h3[hw]==hash2Crack3){
			msgC=1;
		}
	}
}

uint32_t rand_int (int rnd_seed)
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


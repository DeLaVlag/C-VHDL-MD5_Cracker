#include "main.h"

uint32_t h0 , h1 , h2, h3;
static int rnd_seed=1;

//static uint32_t h[4]={0x67452301,0xefcdab89,0x98badcfe,0x10325476};

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

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

void md5_hasher(uint32_t *o0,uint32_t *o1,uint32_t *o2,uint32_t *o3,
		uint32_t *o4,uint32_t *o5,uint32_t *o6,uint32_t *o7,uint32_t *o8,
		uint32_t *o9,uint32_t *o10,uint32_t *o11,uint32_t *o12,
		uint32_t *o13,uint32_t *o14,uint32_t *o15){
//void md5_hasher(void){
//void md5_hasher(uint8_t msg2Hash[64], uint32_t *out0, uint32_t *out1, uint32_t *out2, uint32_t *out3){

#pragma HLS INTERFACE ap_ctrl_none port=return
//#pragma HLS INTERFACE ap_memory port=msg2Hash
//#pragma HLS INTERFACE m_axi port=msg2Hash

//#pragma HLS interface s_axilite bundle=add_io port=out0
//#pragma HLS interface s_axilite bundle=add_io port=out1
//#pragma HLS interface s_axilite bundle=add_io port=out2
//#pragma HLS interface s_axilite bundle=add_io port=out3

#pragma HLS interface s_axilite bundle=output port=o0
#pragma HLS interface s_axilite bundle=output port=o1
#pragma HLS interface s_axilite bundle=output port=o2
#pragma HLS interface s_axilite bundle=output port=o3
#pragma HLS interface s_axilite bundle=output port=o4
#pragma HLS interface s_axilite bundle=output port=o5
#pragma HLS interface s_axilite bundle=output port=o6
#pragma HLS interface s_axilite bundle=output port=o7
#pragma HLS interface s_axilite bundle=output port=o8
#pragma HLS interface s_axilite bundle=output port=o9
#pragma HLS interface s_axilite bundle=output port=o10
#pragma HLS interface s_axilite bundle=output port=o11
#pragma HLS interface s_axilite bundle=output port=o12
#pragma HLS interface s_axilite bundle=output port=o13
#pragma HLS interface s_axilite bundle=output port=o14
#pragma HLS interface s_axilite bundle=output port=o15


//#pragma HLS PIPELINE
//#pragma HLS RESOURCE variable=msg2Hash core=RAM_1P_BRAM metadata="-bus_bundle BUS_CTRL"
//#pragma HLS DEPENDENCE variable=msg2Hash array inter false

// uint32_t h0 , h1 , h2, h3;
//static uint32_t h[4]={0x67452301,0xefcdab89,0x98badcfe,0x10325476};

	//IHVS0
	h0 = 0x67452301;
	h1 = 0xefcdab89;
	h2 = 0x98badcfe;
	h3 = 0x10325476;

	//original
//	uint32_t hash2Crack1 = 0x3f730df4;
//	uint32_t hash2Crack2 = 0xbc2ec291;
//	uint32_t hash2Crack3 = 0xa0d6e5d9;
//	uint32_t hash2Crack4 = 0x73a78511;

	//reversed for ez comparison
	uint32_t hash2Crack0 = 0xf40d733f;
	uint32_t hash2Crack1 = 0x91c22ebc;
	uint32_t hash2Crack2 = 0xd9e5d6a0;
	uint32_t hash2Crack3 = 0x1185a773;

	uint8_t msg2Hash[64] = "I just need a message that's 64B long such to hash 512b";
	msg2Hash[55]=0x80;
	msg2Hash[56]=0xB8;
	msg2Hash[57]=1;
	msg2Hash[58]=0;
	msg2Hash[59]=0;
	msg2Hash[60]=0;
	msg2Hash[61]=0;
	msg2Hash[62]=0;
	msg2Hash[63]=0;

	bool hashFound = 0;
	uint64_t hashcounter=MAX64;

	uint32_t a = h0, b = h1, c = h2, d = h3, x[16];

	//random message generation for birthdaying
	for (uint8_t i=0;i<55;i++){
		int x = rand_int()%256;
		msg2Hash[i]=x;
//		printf("%d ",msg2Hash[i]);
		set_rnd_seed(x);
	}

	while(hashcounter--&&hashFound==0){
//#pragma HLS PIPELINE
		for (uint8_t i = 0, j = 0; j < 64; i++, j += 4)
			x[i] = ((uint32_t)msg2Hash[j]) | (((uint32_t)msg2Hash[j+1]) << 8) |
				(((uint32_t)msg2Hash[j+2]) << 16) | (((uint32_t)msg2Hash[j+3]) << 24);

		/* Round 1 */
		FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
		FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
		FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
		FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
		FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
		FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
		FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
		FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
		FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
		FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
		FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
		FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
		FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
		FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
		FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
		FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

		/* Round 2 */
		GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
		GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
		GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
		GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
		GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
		GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
		GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
		GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
		GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
		GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
		GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
		GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
		GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
		GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
		GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
		GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

		/* Round 3 */
		HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
		HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
		HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
		HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
		HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
		HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
		HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
		HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
		HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
		HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
		HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
		HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
		HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
		HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
		HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
		HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

		/* Round 4 */
		II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
		II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
		II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
		II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
		II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
		II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
		II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
		II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
		II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
		II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
		II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
		II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
		II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
		II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
		II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
		II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;

	//	*out0 = h0;
	//	*out1 = h1;
	//	*out2 = h2;
	//	*out3 = h3;

		uint32_t t_h0 = 0;
		uint32_t t_h1 = 0;
		uint32_t t_h2 = 0;
		uint32_t t_h3 = 0;

		t_h0 = ((h0 & 0xFF)<<3*8) | ((h0 & 0xFF00) << 8) | ((h0 & 0xFF0000) >> 8) | ((h0 & 0xFF000000) >> 3*8);
		t_h1 = ((h1 & 0xFF)<<3*8) | ((h1 & 0xFF00) << 8) | ((h1 & 0xFF0000) >> 8) | ((h1 & 0xFF000000) >> 3*8);
		t_h2 = ((h2 & 0xFF)<<3*8) | ((h2 & 0xFF00) << 8) | ((h2 & 0xFF0000) >> 8) | ((h2 & 0xFF000000) >> 3*8);
		t_h3 = ((h3 & 0xFF)<<3*8) | ((h3 & 0xFF00) << 8) | ((h3 & 0xFF0000) >> 8) | ((h3 & 0xFF000000) >> 3*8);


		printf("hash=%08x%08x%08x%08x\n",t_h0, t_h1, t_h2, t_h3);
	//	printf("hash=%08x%08x%08x%08x\n",h0, h1, h2, h3);

		if (h0==hash2Crack0&&h1==hash2Crack2&&h2==hash2Crack2&&h3==hash2Crack3){
			hashFound=1;
			*o0=x[0];
			*o1=x[1];
			*o2=x[2];
			*o3=x[3];
			*o4=x[4];
			*o5=x[5];
			*o6=x[6];
			*o7=x[7];
			*o8=x[8];
			*o9=x[9];
			*o10=x[10];
			*o11=x[11];
			*o12=x[12];
			*o13=x[13];
			*o14=x[14];
			*o15=x[15];
		}
	}
}

void set_rnd_seed (uint32_t new_seed)
{
    rnd_seed = new_seed;
}

int rand_int (void)
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

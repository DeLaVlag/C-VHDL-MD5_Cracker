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
	t_function = a + x[t_number] + k[index];
	t_function = ROTATE_LEFT(t_function,r[index]);

	//Switch the inputs around a= d, b =a, c= b, d= c
	uint32_t tmp = d;
	d = c;
	c = b;
	b += t_function;
	a = tmp;




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

	uint8_t msg2Hash[64] = "Hello World!";
	msg2Hash[12]=0x80;
	msg2Hash[56]=96;
	msg2Hash[57]=0x00;
	msg2Hash[58]=0;
	msg2Hash[59]=0;
	msg2Hash[60]=0;
	msg2Hash[61]=0;
	msg2Hash[62]=0;
	msg2Hash[63]=0;

	bool hashFound = 0;
	uint64_t hashcounter=1;

	uint32_t a = h0, b = h1, c = h2, d = h3, x[16];

	//random message generation for birthdaying
	for (uint8_t i=0;i<55;i++){
		int x = rand_int()%256;
		msg2Hash[i]=x;
//		printf("%d ",msg2Hash[i]);
		set_rnd_seed(x);
	}

	while(hashcounter--&&hashFound==0){
		for (uint8_t i = 0, j = 0; j < 64; i++, j += 4)
			x[i] = ((uint32_t)msg2Hash[j]) | (((uint32_t)msg2Hash[j+1]) << 8) |
				(((uint32_t)msg2Hash[j+2]) << 16) | (((uint32_t)msg2Hash[j+3]) << 24);


compressLoop:for(int i = 0; i < 64; i++){
#pragma HLS PIPELINE
			md5_compress(a, b, c, d, x, i);
		}

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

		if (h0==hash2Crack0&&h1==hash2Crack1&&h2==hash2Crack2&&h3==hash2Crack3){
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


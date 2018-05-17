#include "main.h"

uint32_t h0, h1, h2, h3;
static int rnd_seed = 1;

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
static uint32_t r[] = { 3, 7, 11, 19, 3, 7, 11, 19, 3, 7, 11, 19, 3, 7, 11, 19,
		3, 5, 9, 13, 3, 5, 9, 13, 3, 5, 9, 13, 3, 5, 9, 13, 3, 9, 11, 15, 3, 9,
		11, 15, 3, 9, 11, 15, 3, 9, 11, 15 };

uint32_t md4_compress(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
		uint32_t x[16], uint8_t index) {
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
	uint8_t t_functionSelector = (index >> 4) & 0b11;
	//Holds the values of the function
	uint32_t t_function = 0;
	uint8_t t_number = 0;
	switch (t_functionSelector) {
	case FF:
		t_function = F(b, c, d);
		t_number = index;
		break;
	case GG:
		t_function = G(b,c,d) + 0x5a827999;
		t_number = ((index * 4) + ((index >> 2) & 0b11)) % 16;
		break;
	case HH:
		t_function = H(b,c,d) + 0x6ed9eba1;
		t_number = (index * 8) % 16;
		t_number += (((index >> 1) & 0b1) * 4) % 16;
		t_number += (((index >> 2) & 0b1) * 2) % 16;
		t_number += ((index >> 3) & 0b1) % 16;
		break;
	default:
		break;
	}
	t_function += a + x[t_number];
	uint8_t rotate = r[index];
	uint32_t value = ROTATE_LEFT(t_function, rotate);
	return value;

}

void md4_hasher(uint32_t x[16], uint8_t msgColl) {

#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE s_axilite port=x
#pragma HLS INTERFACE s_axilite port=msgColl
//#pragma HLS PIPELINE

//IHVS0
	h0 = 0x67452301;
	h1 = 0xefcdab89;
	h2 = 0x98badcfe;
	h3 = 0x10325476;

	//reversed for ez comparison
	uint32_t hash2Crack0 = 0xf40d733f;
	uint32_t hash2Crack1 = 0x91c22ebc;
	uint32_t hash2Crack2 = 0xd9e5d6a0;
	uint32_t hash2Crack3 = 0x1185a773;

//	uint8_t msg2Hash[64] = "I just need a message that's 64B long such to hash 512b";

	uint32_t units = 1;

	uint32_t a = h0, b = h1, c = h2, d = h3; //, x[16];

	for (uint8_t hw = 0; hw < units; hw++) {
#pragma HLS unroll

//		for (uint8_t i = 0; i < 14; i++) {
//			int randval = rand_int();
//			x[i] = randval;
////			printf("%08x ",x[i]);
//		}
//
//		//setting the last part of msg with 1 and sizeof, with reversal
//		x[13] &= ~(0xff000000);
//		x[13] |= (1 << 31);
//		x[14] = 0x1B8;
//		x[15] = 0;

		//reversed is original message!
		printf("\n reversed message1:\n");
		//everything upto the 0x80 is hashed. check online for hexhasher.
		for (uint8_t i = 0; i < 16; i++) {
			printf("%08x",
					((x[i] & 0xFF) << 3 * 8) | ((x[i] & 0xFF00) << 8)
							| ((x[i] & 0xFF0000) >> 8)
							| ((x[i] & 0xFF000000) >> 3 * 8));
		}

		compressLoop: for (int i = 0; i < 48; i++) {
#pragma HLS PIPELINE
			uint32_t result = md4_compress(a, b, c, d, x, i);
			//Switch the inputs around a= d, b =a, c= b, d= c
			uint32_t tmp = d;
			d = c;
			c = b;
			b = result;
			a = tmp;
		}

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;

		//reversing done by software
		h0 = ((h0 & 0xFF) << 3 * 8) | ((h0 & 0xFF00) << 8)
				| ((h0 & 0xFF0000) >> 8) | ((h0 & 0xFF000000) >> 3 * 8);
		h1 = ((h1 & 0xFF) << 3 * 8) | ((h1 & 0xFF00) << 8)
				| ((h1 & 0xFF0000) >> 8) | ((h1 & 0xFF000000) >> 3 * 8);
		h2 = ((h2 & 0xFF) << 3 * 8) | ((h2 & 0xFF00) << 8)
				| ((h2 & 0xFF0000) >> 8) | ((h2 & 0xFF000000) >> 3 * 8);
		h3 = ((h3 & 0xFF) << 3 * 8) | ((h3 & 0xFF00) << 8)
				| ((h3 & 0xFF0000) >> 8) | ((h3 & 0xFF000000) >> 3 * 8);

		printf("\nhash=%08x%08x%08x%08x\n", h0, h1, h2, h3);

		//reversed is original message!
		printf("\n reversed message2:\n");
		//everything upto the 0x80 is hashed. check online for hexhasher.
		for (uint8_t i = 0; i < 16; i++) {
			printf("%08x",
					((x[i] & 0xFF) << 3 * 8) | ((x[i] & 0xFF00) << 8)
							| ((x[i] & 0xFF0000) >> 8)
							| ((x[i] & 0xFF000000) >> 3 * 8));
		}

		//write outputs when hash is found
		if (h0 == hash2Crack0 && h1 == hash2Crack2 && h2 == hash2Crack2
				&& h3 == hash2Crack3) {
			msgColl = hw;
		}
	}
}

uint32_t need_seed(void) {
	unsigned int hi, lo;

	hi = 16807 * (rnd_seed >> 16);
	lo = 16807 * (rnd_seed & 0xFFFF);
	lo += (hi & 0x7FFF) << 16;
	lo += hi >> 15;
	if (lo > 2147483647)
		lo -= 2147483647;
	rnd_seed = lo;

	return rnd_seed;
}

uint32_t rand_int(void) {
	unsigned int hi, lo;

	hi = 16807 * (need_seed() >> 16);
	lo = 16807 * (need_seed() & 0xFFFF);
	lo += (hi & 0x7FFF) << 16;
	lo += hi >> 15;
	if (lo > 2147483647)
		lo -= 2147483647;
	return lo;
}


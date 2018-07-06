
#include <stdint.h>
#include <iostream>

#include "hashtest.h"
#include "main.h"

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define ROTATE_RIGHT(x, n) (((x) >> (n)) | ((x) << (32-(n))))



// helper methods to adjust the state variables
// to satisfy Wang's constraints

/**
 * Sets a bit at bitposition @param position at 1
 * @param v, the state value which we want to change
 * @param state, the register value before a md4 operation was applied.
 * @param position, the position at which we want to change @param v
 * @return the modified value v;
 */
uint32_t correct_bit_EQUALal(uint32_t v, uint32_t state, uint8_t position){
    v ^= ((v^state) & (1 << position));
    return v;
}



/**
 * Sets a bit at bitposition @param position at 0
 * @param v, the state value which we want to change
 * @param position, the position at which we want to change @param v
 * @return the modified value v;
 */
uint32_t correct_bit_zero(uint32_t v, uint8_t position){
    v &= ~(1 << position); 
    return v;
}

/**
 * Sets a bit at bitposition @param position at 1
 * @param v, the state value which we want to change
 * @param position, the position at which we want to change @param v
 * @return the modified value v;
 */
uint32_t correct_bit_one(uint32_t v, uint8_t position){
    v |= (1 << position); 
    return v;
}

#define ZERO 0
#define ONE 1
#define EQUAL 2




uint32_t correct_op(uint32_t v, uint32_t state, uint8_t type, uint8_t constraint ){

    // correct the bits according to the constraints
    switch(type){
        case ZERO:
            v = correct_bit_zero(v, constraint);
            break;
        case ONE:
            v = correct_bit_one(v, constraint);
            break;
        case EQUAL:
            v = correct_bit_EQUALal(v, state, constraint);
            break;
        default:
            break;
    }
    return v;
}


void fillLittleEndian(uint32_t x[16], uint8_t m[64] ){
    int i, j;
    for (i = 0, j = 0; j < 64; i++, j += 4)
		  x[i] = ((uint32_t)m[j]) | (((uint32_t)m[j+1]) << 8) |
		  	  	 (((uint32_t)m[j+2]) << 16) | (((uint32_t)m[j+3]) << 24);
}

void UnravelLittleEndian(uint32_t x[16], uint8_t m[64]){
    int i, j;
    for (i = 0, j = 0; j < 64; i++, j += 4){
        uint32_t temp = x[i];
        m[j] = temp & 0xFF;
        m[j+1] = (temp >> 8) &  0xFF;
        m[j+2] = (temp >> 16) & 0xff ;
        m[j+3] = (temp >> 24) & 0xff;
    }
}


void printArray(uint8_t message[64]){
	for(uint8_t i = 0; i < 64 ; i++){
		printf("%02x", message[i]);
	}
	std::cout << std::endl;
}
void gen_probable_collision(uint32_t x[16]){

    for(int i =0; i < 16 ;i++){
    	x[i] = rand_int();
    }

    	// fillLittleEndian(x, m);
    // md4_hasher(x, 0);

    uint32_t h[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

    uint8_t constraintType[96] = {EQUAL,ZERO,EQUAL,EQUAL,ONE,ONE,ZERO,EQUAL,ONE,ZERO,ZERO,ZERO,ONE,ONE,ZERO,EQUAL,ZERO,EQUAL,EQUAL,EQUAL,EQUAL,ONE,EQUAL,ZERO,EQUAL,ZERO,ZERO,ONE,ZERO,ONE,ONE,ZERO,EQUAL,ZERO,ZERO,ZERO,ZERO,ONE,ONE,ONE,ZERO,ZERO,ZERO,ZERO,EQUAL,ONE,EQUAL,ONE,ONE,ONE,ZERO,ZERO,ONE,ONE,ZERO,ONE,EQUAL,ONE,ZERO,ZERO,ZERO,ZERO,ZERO,ONE,EQUAL,ZERO,ONE,ONE,EQUAL,ONE,ZERO,ZERO,ZERO,ZERO,EQUAL,EQUAL,ONE,ZERO,ZERO,ZERO,ONE,ONE,ZERO,ONE,EQUAL,ONE,ONE,ZERO,ZERO,ZERO,ZERO,EQUAL,ONE,ONE,ZERO,EQUAL};
    uint8_t value[96] = {6,6,7,10,6,7,10,25,6,7,10,25,7,10,25,13,13,18,19,20,21,25,12,13,14,18,19,20,21,12,13,14,16,18,19,20,21,12,13,14,16,18,19,20,22,21,25,12,13,14,16,19,20,21,22,25,29,16,19,20,21,22,25,29,31,19,20,21,22,25,29,31,22,25,26,28,29,31,22,25,26,28,29,31,18,22,25,26,28,29,18,25,26,28,29,31 };

    uint8_t lengths[16] = {1, 3, 4, 4, 4, 6, 7, 8, 10, 10, 8, 7, 6, 6, 6, 6};
    uint8_t rotations[4] = {3, 7, 11, 19};

    uint32_t v = 0;
    uint8_t start = 0;

	//Applies the first set of changes to the message.
    for(int i = 0; i < 16; i++){
		
        uint8_t index = 16-i;
        uint32_t t_func = F(h[(index+1)%4], h[(index+2)%4], h[(index+3)%4]);
		
        v = ROTATE_LEFT(h[index%4] + t_func +x[i], rotations[(i%4)]); // do one round of F function
        
        //Apply the changes to the output of a round.
        for(int j =start; j < start+lengths[i]; j++){
            v = correct_op(v, h[(index +1)%4], constraintType[j], value[j]);
        }
		//Reflect changes on the output, on the input
        x[i] = ROTATE_RIGHT(v, rotations[(i%4)]) - h[(index)%4] -t_func;
        
		start +=lengths[i];
        h[(index%4)] =v;

    }

    uint8_t constraintType2[7] = {EQUAL, ONE, ZERO, ONE, ONE, EQUAL, EQUAL};
    uint8_t values[7] = {18, 25, 26, 28, 31, 28, 31};
    uint8_t length[2] = {5, 2};
	
    uint32_t ax = ROTATE_LEFT(h[0] + G(h[1], h[2], h[3])  +x[0]+ 0x5a827999, 3);
    uint32_t q = ax;

    for(int j =0; j < length[0]; j++){
            ax = correct_op(ax, h[2], constraintType2[j], values[j]);
    }

    q = ROTATE_RIGHT(ax, 3) - h[0] - G(h[1], h[2], h[3]) - 0x5a827999;

    uint32_t a0 = 0x67452301, b0= 0xefcdab89, c0 = 0x98badcfe, d0 = 0x10325476;

    uint32_t tmp[16]= {q};
    uint32_t a1prime = md4_compress(a0,b0,c0,d0, tmp, 0);
    uint32_t a1 = md4_compress(a0,b0,c0,d0, x, 0);
    uint32_t d1 = md4_compress(d0,a1,b0,c0, x, 1);
    uint32_t c1 = md4_compress(c0,d1,a1,b0, x, 2);
    uint32_t b1 = md4_compress(b0,c1,d1,a1, x, 3);
    uint32_t a2 = md4_compress(a1,b1,c1,d1, x, 4);


    x[0] = q;
    q = x[1];
    x[1] = (ROTATE_RIGHT(d1,  7) - d0 - F(a1prime, b0, c0));
    x[2] = (ROTATE_RIGHT(c1, 11) - c0 - F(d1, a1prime, b0));
    x[3] = (ROTATE_RIGHT(b1, 19) - b0 - F(c1, d1, a1prime));
    x[4] = (ROTATE_RIGHT(a2,  3) - a1prime - F(b1, c1, d1));

    h[0] = ax;

    uint32_t d5 = md4_compress(h[3], h[0], h[1], h[2], x, 17);
     for(int j =length[0]; j < length[0]+length[1]; j++){
            ax = correct_op(ax, h[1], constraintType[j], value[j]);
    }

    q = (ROTATE_RIGHT(d5, 5) - h[3] - G(h[0], h[1], h[2])- 0x5a827999);


    uint32_t a = 0x67452301, b= 0xefcdab89, c = 0x98badcfe, d = 0x10325476;

    a = md4_compress(a,b,c,d, x, 0);
    d = md4_compress(d,a,b,c, x, 1);
    c = md4_compress(c,d,a,b, x, 2);
    b = md4_compress(b,c,d,a, x, 3);

    tmp[4] = q;
    uint32_t a2prime = md4_compress(a,b,c,d, tmp, 4);
    a2 = md4_compress(a,b,c,d, x, 4);
    uint32_t d2 = md4_compress(d,a2,b,c, x, 5);
    uint32_t c2 = md4_compress(c,d2,a2,b, x, 6);
    uint32_t b2 = md4_compress(b,c2,d2,a2, x, 7);
    uint32_t a3 = md4_compress(a2,b2,c2,d2, x, 8);


    x[4] = q;
    x[5] = (ROTATE_RIGHT(d2,  7) - d - F(a2prime, b, c));
    x[6] = (ROTATE_RIGHT(c2, 11) - c - F(d2, a2prime, b));
    x[7] = (ROTATE_RIGHT(b2, 19) - b - F(c2, d2, a2prime));
    x[8] = (ROTATE_RIGHT(a3,  3) - a2prime - F(b2, c2, d2));

}

void createAdaptedMessage(uint32_t x[16], uint32_t xprime[16]){

    for(int i = 0; i < 16; i++){
        xprime[i] = x[i];
    }

    xprime[1] = (xprime[1] + (1 << 31));
    xprime[2] = (xprime[2] + ((1 << 31) - (1 << 28)));
    xprime[12] = (xprime[12] - (1 << 16));

}

bool hashit(uint32_t x[16], uint32_t xprime[16]){

	if (x[0] != 0){
	uint32_t signaturePrime[4] = {0x67452301,0xefcdab89, 0x98badcfe, 0x10325476};
	uint32_t signatureX[4] = {0x67452301,0xefcdab89, 0x98badcfe, 0x10325476};
	uint32_t messagex[16] = {0};

    md4_hasher(xprime, signaturePrime, 0);
    md4_hasher(x, signatureX, 0);

    if (signaturePrime[0] == signatureX[0] && signaturePrime[1] == signatureX[1] && signaturePrime[2] == signatureX[2]&& signaturePrime[3] == signatureX[3]) {
    	std::cout << std::endl;
    	std::cout << "Match found " << std::endl;
        printf("%08x%08x%08x%08x\n",signaturePrime[0] , signaturePrime[1] , signaturePrime[2] ,signaturePrime[3] );

    	return true;
    }
	}
	return false;

}


int test(){
#pragma HLS INTERFACE s_axilite port=return

	uint32_t xprime[16] = {0};
	uint32_t x[16] = {};//fillxregister

	while(!hashit(x, xprime)){
		gen_probable_collision(x);
		createAdaptedMessage(x, xprime);
	}

    return 0;
}

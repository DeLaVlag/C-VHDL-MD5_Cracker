#include "hashtest.h"
#include <math.h>


int main() {

//	uint8_t msg2Hash[] =
//			"I just need a message that's 64B long such to hash 512b";
//	msg2Hash[55]=0x80;
//	msg2Hash[56]=0xB8;
//	msg2Hash[57]=1;
//	msg2Hash[58]=0;
//	msg2Hash[59]=0;
//	msg2Hash[60]=0;
//	msg2Hash[61]=0;
//	msg2Hash[62]=0;
//	msg2Hash[63]=0;
//	msg2Hash[64]=0;
//
//	uint32_t msgLen = sizeof(msg2Hash) / sizeof(msg2Hash[0]) - 1;
////	printf("hasjLen = %d",msgLen);
//
//	uint32_t out0,out1,out2,out3;
//
////The input message is broken up into chunks of 512-bit blocks (sixteen 32-bit words);
////the message is padded so that its length is divisible by 512.
////The padding works as follows:
////first a single bit, 1, is appended to the end of the message.
////This is followed by as many zeros as are required to bring the length of the message up to 64 bits fewer than a multiple of 512.
////The remaining bits are filled up with 64 bits representing the length of the original message, modulo 264.
//
//
//	uint32_t padLen = (msgLen % 64) < 56? (56 - (msgLen%64)): (120 - (msgLen%64));
//	uint32_t paddedMessageLen= (msgLen + padLen + 8);
//	uint8_t* paddedMessage= (uint8_t*)malloc(sizeof(uint8_t)*(msgLen + padLen + 8));
//
//	uint16_t round = (uint16_t) paddedMessageLen / 64;
//
//
//	for (int i = 0 ; i < msgLen; i++){
//		paddedMessage[i] = msg2Hash[i];
//	}
//	paddedMessage[msgLen] = 0x80;
//	for(int i = msgLen+1; i < msgLen+padLen; i++){
//		paddedMessage[i] = 0;
//	}
//
//	uint64_t length = msgLen * 8;
//	for (int i = msgLen + padLen, k = 0; i < msgLen + padLen + 8; i++, k++) {
//		paddedMessage[i] = 0xFF & (length >> k * 8);
//		printf("%d",paddedMessage[i]);
//	}
//
//	for (int j = 0; j < round; j++) {
//		uint8_t input[64] = { 0 };
//
//		for (int i = 0; i < 64; i++) {
//			input[i] = paddedMessage[j*64+i];
//		}
//		//main function call
////		md5_hasher(input);
////		md5_hasher(input,&out0,&out1,&out2,&out3);
//	}

	uint32_t o0,o1,o2,o3,o4,o5,o6,o7,o8,o9,o10,o11,o12,o13,o14,o15;
	md5_hasher(&o0,&o1,&o2,&o3,&o4,&o5,&o6,&o7,&o8,&o9,&o10,&o11,&o12,&o13,&o14,&o15);

//	md5_printer();
//	md5_printer(&out0,&out1,&out2,&out3);
}
//
//void md5_printer(){
////void md5_printer(uint32_t *out0,uint32_t *out1,uint32_t *out2,uint32_t *out3){
//	uint32_t t_h0 = 0;
//	uint32_t t_h1 = 0;
//	uint32_t t_h2 = 0;
//	uint32_t t_h3 = 0;
//	unsigned int i;
//
//	t_h0 = ((h0 & 0xFF)<<3*8) | ((h0 & 0xFF00) << 8) | ((h0 & 0xFF0000) >> 8) | ((h0 & 0xFF000000) >> 3*8);
//	t_h1 = ((h1 & 0xFF)<<3*8) | ((h1 & 0xFF00) << 8) | ((h1 & 0xFF0000) >> 8) | ((h1 & 0xFF000000) >> 3*8);
//	t_h2 = ((h2 & 0xFF)<<3*8) | ((h2 & 0xFF00) << 8) | ((h2 & 0xFF0000) >> 8) | ((h2 & 0xFF000000) >> 3*8);
//	t_h3 = ((h3 & 0xFF)<<3*8) | ((h3 & 0xFF00) << 8) | ((h3 & 0xFF0000) >> 8) | ((h3 & 0xFF000000) >> 3*8);
//
//
//	printf("hash=%08x%08x%08x%08x\n",t_h0, t_h1, t_h2, t_h3);
//}


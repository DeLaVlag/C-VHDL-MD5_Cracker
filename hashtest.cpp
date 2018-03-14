#include "hashtest.h"
#include <math.h>

int main() {

	uint8_t msg2Hash[] =
			"Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow Hello Yellow";

//	char *msg = msg2Hasj[12];
	uint32_t msgLen = sizeof(msg2Hash) / sizeof(msg2Hash[0]) - 1;
//	printf("hasjLen = %d",hasjLen);

//The input message is broken up into chunks of 512-bit blocks (sixteen 32-bit words);
//the message is padded so that its length is divisible by 512.
//The padding works as follows:
//first a single bit, 1, is appended to the end of the message.
//This is followed by as many zeros as are required to bring the length of the message up to 64 bits fewer than a multiple of 512.
//The remaining bits are filled up with 64 bits representing the length of the original message, modulo 264.


	uint32_t padLen = (msgLen % 64) < 56? (56 - (msgLen%64)): (120 - (msgLen%64));
	uint32_t paddedMessageLen= (msgLen + padLen + 8);
	uint8_t* paddedMessage= (uint8_t*)malloc(sizeof(uint8_t)*(msgLen + padLen + 8));

	uint16_t round = (uint16_t) paddedMessageLen / 64;


	for (int i = 0 ; i < msgLen; i++){
		paddedMessage[i] = msg2Hash[i];
	}
	paddedMessage[msgLen] = 0x80;
	for(int i = msgLen+1; i < msgLen+padLen; i++){
		paddedMessage[i] = 0;
	}

	uint64_t length = msgLen * 8;
	for (int i = msgLen + padLen, k = 0; i < msgLen + padLen + 8; i++, k++) {
		paddedMessage[i] = 0xFF & (length >> k * 8);
	}

	for (int j = 0; j < round; j++) {
		uint8_t input[64] = { 0 };

		for (int i = 0; i < 64; i++) {
			input[i] = paddedMessage[j*64+i];
		}
		md5_hasher(input);
	}

	//main function call

	md5_printer();
}


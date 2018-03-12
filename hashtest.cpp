#include "hashtest.h"

int main(){

	uint8_t msg2Hash[] = "Hello Yellow Hello Yellow";

//	char *msg = msg2Hasj[12];
	uint32_t msgLen = sizeof(msg2Hash)/sizeof( msg2Hash[0])-1;
//	printf("hasjLen = %d",hasjLen);

	//The input message is broken up into chunks of 512-bit blocks (sixteen 32-bit words);
	//the message is padded so that its length is divisible by 512.
	//The padding works as follows:
	//first a single bit, 1, is appended to the end of the message.
	//This is followed by as many zeros as are required to bring the length of the message up to 64 bits fewer than a multiple of 512.
	//The remaining bits are filled up with 64 bits representing the length of the original message, modulo 264.

	uint32_t padLen = (msgLen < 56)?(56-msgLen):(120-msgLen);

	uint8_t input[64] = {0};
	for(int i = 0; i < 56-padLen; i++){
		input[i] = msg2Hash[i];
	}

	input[56-padLen] = 0x80;
	uint64_t length = msgLen*8;
	for(int i = 56, j= 0; i < 64; i++, j++){

		input[i] =0xFF& (length << j*8 );
	}

	//main function call
	md5_hasher(input);

}

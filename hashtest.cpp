#include "hashtest.h"

int main(){

	uint8_t msg2Hasj[] = "Hello Yellow";

//	char *msg = msg2Hasj[12];
	uint32_t hasjLen = sizeof(msg2Hasj)/sizeof( msg2Hasj[0])-1;
	printf("hasjLen = %d",hasjLen);

	md5_hasher(msg2Hasj, hasjLen);


}

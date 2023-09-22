#include "eth.h"
#include <stdio.h>
#include <assert.h>

int main(){
	uint8_t t[1] = {0};

	eth_packet_s *pkt;	

	printf("Running test\n");
	
	/* test crc */
	uint32_t crc = calculate_crc(t, 1);
	assert(crc == 0x4E08BFB4);

	printf("PASS: data { 0 } crc result : %08x\n", crc);

	return 0;
}

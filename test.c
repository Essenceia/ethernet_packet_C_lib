#include "eth.h"
#include <stdio.h>
#include <assert.h>
#include "dump.h"

int main(){
	uint8_t t[1] = {0};

	eth_packet_s *pkt;	

	printf("Running test\n");
	
	/* test crc */
	uint32_t crc = calculate_crc(t, 1);
	assert(crc == 0x4E08BFB4);

	printf("PASS: data { 0 } crc result : %08x\n", crc);

	/* create packet */
	pkt = init_eth_packet(
		DEFAULT_DST_MAC,
		DEFAULT_SRC_MAC,
		DEFAULT_SRC_IP,
		DEFAULT_DST_IP,
		DEFAULT_SRC_PORT,
		DEFAULT_DST_PORT,
		false);
	
	print_eth_packet(pkt);

	/* dump packet to file */
	size_t dump_len;
	uint8_t *dump = write_eth_packet(
			pkt,
			&dump_len);
 
	dump_eth_packet(
		dump, 
		dump_len, 
		true);

	free_eth_packet(pkt);

	return 0;
}

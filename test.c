/* Copyright (c) 2023, Julia Desmazes. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial
 * 4.0 International License.
 *
 * This code is provided "as is" without any express or implied warranties. */

#include "eth.h"
#include <stdio.h>
#include <assert.h>
#include "dump.h"
#include "defs.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#define PKT_LEN_MAX 1800
#define PKT_LEN_MIN 30
#define NODE_CNT 2
#define PKT_CNT 10

int main(){
	uint8_t t[1] = {0};


	printf("Running test\n");
	
	/* test crc */
	uint32_t crc = calculate_crc(t, 1);
	assert(crc == 0x4E08BFB4);

	printf("PASS: data { 0 } crc result : %08x\n", crc);

	/* create node structs */
	
	eth_packet_s *node[NODE_CNT];	

	for(int i=0; i < NODE_CNT; i++){
		node[i] = init_eth_packet(
			( i % 2) ? DEFAULT_DST_MAC: DEFAULT_SRC_MAC ,
			( i % 2) ? DEFAULT_SRC_MAC: DEFAULT_DST_MAC,
			( i % 2) ? DEFAULT_SRC_IP: DEFAULT_DST_IP ,
			( i % 2) ? DEFAULT_DST_IP: DEFAULT_SRC_IP ,
			( i % 2) ? DEFAULT_SRC_PORT: DEFAULT_DST_PORT,
			( i % 2) ? DEFAULT_DST_PORT: DEFAULT_SRC_PORT,
			false);
		print_eth_packet(node[i]);
	}

	/* fill packets and dump packet to file */
	uint8_t data[PKT_LEN_MAX];
	size_t data_len;
	size_t dump_len;
	uint8_t *dump;
	
	srand((unsigned int)time(NULL));   
	int r;
	int n;
	for(int i=0; i<PKT_CNT; i++){
		#ifdef DEBUG
		printf("creating packet %d\n",i);
		#endif
		r = rand();
		/* randomly select a node */
		n = r % NODE_CNT;
		/* fill packet with fake data */
		data_len = (size_t)(r % (PKT_LEN_MAX-PKT_LEN_MIN) + PKT_LEN_MIN);
		for(size_t l=0; l < data_len; l++){
			/* TODO: fill with macbeth */
			data[l] = (uint8_t) rand();
		}	
		/* update node packet content */
		update_eth_packet_data(node[n],data, data_len);
		/* dump */
		dump = write_eth_packet(
				node[n],
				&dump_len);
		#ifdef DEBUG
		print_eth_packet(node[n]);
		#endif
		dump_eth_packet(
			dump, 
			dump_len, 
			true);
		free(dump);
	}
	
	for(int i=0; i<NODE_CNT; i++)
		free_eth_packet(node[i]);

	close_dump();
	
	printf("End of test\n");
	return 0;
}

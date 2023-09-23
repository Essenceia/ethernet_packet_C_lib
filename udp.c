#include "eth_defs.h"
#include "udp.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* UDP */
udp_head_s *read_udp_head(uint8_t *buff, size_t len){
	assert(buff);
	assert(len>= UDP_HEAD_SIZE);
	
	udp_head_s * head = malloc(sizeof(udp_head_s));
	memcpy(head, buff, sizeof(udp_head_s) );
	
	return head;
}

void set_udp_len(udp_head_s *head, size_t data_len){
	assert(head);
	head->len = (uint16_t)(UDP_HEAD_SIZE + data_len);			
}

uint8_t *write_udp_head(udp_head_s* head, size_t *len){
	assert(head);
	*len = UDP_HEAD_SIZE;
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memcpy(buff, head, (*len));
	for(int i=0; i < 4; i++){
		uint16_t tmp; 
		memcpy(&tmp, buff+(i*2),sizeof(uint16_t));
		tmp = htobe16(tmp);
		memcpy(buff+(i*2),&tmp, sizeof(uint16_t));
	}
	return buff;	
} 

udp_head_s * init_udp_head(
	const uint16_t src_port,
	const uint16_t dst_port,
	const uint16_t udp_data_len
){
	udp_head_s *head;
	head = malloc(sizeof(udp_head_s));
	head->src_port = src_port;
	head->dst_port = dst_port;
	head->len = UDP_HEAD_SIZE + udp_data_len;
	head->cs = 0;

	return head;
}

void print_udp_head(udp_head_s * head){
	assert(head);
	printf("udp head {\n");
	printf("\tsrc port : %u\n", head->src_port);
	printf("\tdst port : %u\n", head->dst_port);
	printf("\tlen : %u\n", head->len);
	printf("\tcs : %u\n", head->cs);

}
	

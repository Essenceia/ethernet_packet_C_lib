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
	head->len = (uint32_t)(UDP_HEAD_SIZE + data_len);			
}

uint8_t *write_udp_head(udp_head_s* head, size_t *len){
	assert(head);
	*len = UDP_HEAD_SIZE;
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memcpy(buff, head, (*len));
	return buff;	
} 

udp_head_s * init_udp_head(
	const uint32_t src_port,
	const uint32_t dst_port,
	const uint32_t udp_data_len
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
	

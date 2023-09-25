#include "tcp.h"


tcp_head_s *read_tcp_head(uint8_t *buff, size_t len){
	assert(buff);
	assert(len >= TCP_HEAD_SIZE);
	
	/* read buffer content and write into tcp header struct */
	tcp_head_s *head = malloc(sizeof(tcp_head_s));
	size_t off = 0;
	/* src/dst port */
	memcpy(&(head->src_port),buff, sizeof(uint16_t));
	off += sizeof(uint16_t);
	memcpy(&(head->dst_port),buff+off, sizeof(uint16_t));
	off += sizeof(uint16_t);

	/* seq num */
	memcpy(&(head->seq_num),buff+off, sizeof(uint32_t));
	off += sizeof(uint32_t);
	/* ack num */
	memcpy(&(head->ack_num),buff+off, sizeof(uint32_t));
	off += sizeof(uint32_t);

	/* data off */
	memcpy(&(head->data_off), buff+off, sizeof(uint8_t));
	off += sizeof(uint8_t);

	/* 1 bit wide parameters */
	uint8_t tmp;
	memcpy(&tmp, buff+off, sizeof(uint8_t));
	head->cwr = (uint8_t)0x1 & tmp;
	head->ece = (uint8_t)0x1 & (tmp >> 1);
	head->urg = (uint8_t)0x1 & (tmp >> 2);
	head->ack = (uint8_t)0x1 & (tmp >> 3);
	head->psh = (uint8_t)0x1 & (tmp >> 4);
	head->rst = (uint8_t)0x1 & (tmp >> 5);
	head->syn = (uint8_t)0x1 & (tmp >> 6);
	head->fin = (uint8_t)0x1 & (tmp >> 7);
	off += sizeof(uint8_t);

	/* window size */
	memcpy(&(head->win_size),buff, sizeof(uint16_t));
	off += sizeof(uint16_t);
	memcpy(&(head->cs),buff+off, sizeof(uint16_t));
	off += sizeof(uint16_t);
	memcpy(&(head->urg_ptr),buff+off, sizeof(uint16_t));
	off += sizeof(uint16_t);

	assert(off == TCP_HEAD_SIZE);

	return head;
}

uint8_t *write_tcp_head(tcp_head_s *head, size_t *len){
	assert(head);

	uint8_t *buff = malloc(sizeof(uint8_t)*TCP_HEAD_SIZE);
	size_t cpy_len = OFFOF(tcp_head_s, data_off); 
	memcpy(buff, head, cpy_len);

	size_t off = cpy_len;			
	buff[off] = (head->data_off & 0xf) 
			  | ((head->res << 4) & 0xf0);
	off += 1;
	buff[off] = head->cwr
		      |(head->ece << 1)	
		      |(head->urg << 2)	
		      |(head->ack << 3)	
		      |(head->psh << 4)	
		      |(head->rst << 5)	
		      |(head->syn << 6)	
		      |(head->fin << 7);
	off += 1;
	memcpy(buff+off, &(head->win_size),sizeof(uint16_t);
	off += sizeof(uint16_t);
	memcpy(buff+off, &(head->cs),sizeof(uint16_t);
	off += sizeof(uint16_t);
	memcpy(buff+off, &(head->urg_ptr),sizeof(uint16_t);
	off += sizeof(uint16_t);

	assert(off == TCP_HEAD_SIZE);
	
	(*len) = off;
	return buff;	
}


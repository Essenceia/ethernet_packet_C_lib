/* Copyright (c) 2023, Julia Desmazes. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial
 * 4.0 International License.
 *
 * This code is provided "as is" without any express or implied warranties. */

#include "tcp.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "eth_defs.h"
#include "defs.h"

tcp_head_s *read_tcp_head(uint8_t *buff, size_t len){
	assert(buff);
	assert(len >= TCP_HEAD_SIZE);
	
	/* read buffer content and write into tcp header struct */
	tcp_head_s *head = malloc(sizeof(tcp_head_s));
	
	/* copy memory */
	memcpy(head,buff, sizeof(tcp_head_s));

	assert(sizeof(tcp_head_s) == TCP_HEAD_SIZE);

	return head;
}

uint8_t *write_tcp_head(tcp_head_s *head, size_t *len){
	assert(head);

	size_t off = sizeof(tcp_head_s);
	uint8_t *buff = malloc(off);
	memcpy(buff, head, off);

	assert(off == TCP_HEAD_SIZE);
	
	(*len) = off;
	return buff;	
}

tcp_head_s * init_tcp_head(
	const uint16_t src_port,
	const uint16_t dst_port,
	const uint16_t data_len,
	const uint8_t  flags
){
	tcp_head_s * head = malloc(sizeof(tcp_head_s));
	memset(head, 0, sizeof(tcp_head_s));

	head->src_port = src_port;
	head->dst_port = dst_port;

	/* set flags */
	head->flags = flags;

	return head;
}	



/* Copyright (c) 2023, Julia Desmazes. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial
 * 4.0 International License.
 *
 * This code is provided "as is" without any express or implied warranties. */

#include "mac.h"
#include "eth_defs.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _BSD_SOURCE             /* See feature_test_macros(7) */
#include <endian.h>

/* MAC 
 * header */
mac_head_s *read_mac_head(uint8_t *buff, size_t len){
	assert(buff);
	/* min mac header length */
	assert(len>=MAC_HEAD_TAG_SIZE);

	mac_head_s *head = malloc(sizeof(mac_head_s));
	const size_t cpy_size = MAC_HEAD_BASE_LITE_SIZE;
	memcpy(head, buff, cpy_size);
	uint16_t type;
	memcpy(&type, buff+cpy_size, sizeof(type));
	if ( type == MAC_TPID ){
		// have tag
		memcpy(((uint8_t*)head)+cpy_size, buff+cpy_size, 6 );
	}else{
		head->type = type;
	}
	return head;
}
/* flatten mac header content and set correct endianness */
uint8_t *write_mac_head(mac_head_s *head, size_t *len){
	assert(head);

	/* calculate mac header length */
	*len = get_mac_head_len(head);
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));

	/* copy preamble, src and dst address into buffer  */
	memcpy(buff, head, MAC_HEAD_BASE_LITE_SIZE);
	
	/* add type indenpendantly as we need to set correct endianness*/
	uint16_t type_be = htobe16(head->type);
	uint16_t tpid_be = htobe16(head->tpid);
	uint16_t tci_be = htobe16(head->tci);

	size_t off = MAC_HEAD_BASE_LITE_SIZE;	
	if(mac_has_tag(head)){
		/* has tag */
		memcpy(buff+off, &tpid_be, sizeof(uint16_t));	
		off += sizeof(uint16_t);
		memcpy(buff+off, &tci_be, sizeof(uint16_t));	
		off += sizeof(uint16_t);
	}
	/* write type type */
	memcpy(buff+off, &type_be, sizeof(uint16_t));	
	
	return buff;
}

bool mac_has_tag(mac_head_s *head){
	assert(head);
	return head->tpid == MAC_TPID;
}

size_t get_mac_head_len(mac_head_s *head){
	/* check if we have a tag or not */
	assert(head);
	size_t data_len = MAC_HEAD_BASE_SIZE;
	data_len +=  mac_has_tag(head) ? MAC_HEAD_TAG_SIZE : 0;
	return data_len;
}

/* foot */

mac_foot_s *read_mac_foot(uint8_t *buff, size_t len){
	assert(buff);
	assert(len >= MAC_FOOT_SIZE);
	mac_foot_s *foot = malloc(sizeof(mac_foot_s));
	memcpy(foot, buff, sizeof(mac_foot_s));
	return foot;
}
uint8_t *write_mac_foot(mac_foot_s *foot, size_t *len){
	assert(foot);
	*len = MAC_FOOT_SIZE; // 32b 
	uint32_t crc_be = htobe32(foot->crc);
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memcpy( buff, &crc_be, sizeof(uint8_t)*(*len));
	return buff;
}

/* Calculate crc */
uint32_t calculate_crc(uint8_t *buff, size_t len){
	uint32_t crc = 0xffffffff;
	uint32_t b; 
	#ifdef DEBUG_CRC
	printf("MAC CRC\n");
	#endif
	for (size_t j=0; j<len; j++){
		b = (uint32_t)buff[j];
		crc ^= b << 24;
		for(size_t i = 0; i < 8; i++ ){
			if((crc & 0x80000000) != 0){
				crc = (crc << 1) ^ 0x04C11DB7;
		 	}else{
				crc <<= 1;
			}
		}
		#ifdef DEBUG_CRC
		printf("j : %ld, b : 0x%02x, crc 0x%08x\n",j,b,crc);
		#endif
	}
	#ifdef DEBUG_CRC
	printf("crc32 : 0x%08x\n",crc);
	#endif
	return crc;
}

mac_head_s *init_mac_head(
	const uint8_t dst_addr[6],
	const uint8_t src_addr[6],
	const bool vtag
){

	info("Init mac head\n");

	mac_head_s *mac = malloc(sizeof(mac_head_s));
	memset(mac->pre,0x55, 7);
	mac->sfd = 0xd5;
	memcpy(mac->dst_addr, dst_addr, 6);
	memcpy(mac->src_addr, src_addr, 6);
	if ( vtag ){
		mac->tpid = MAC_TPID;
		mac->tci = DEFAULT_TCI_MAC;
	}else{
		mac->tpid = 0;
	}
	/* IPv4 */
	mac->type = MAC_TYPE;
	
	return mac; 
}

mac_foot_s *init_mac_foot(){
	mac_foot_s *foot = malloc(sizeof(mac_foot_s));
	return foot;
}

void print_mac_head(mac_head_s *head){
	printf("mac head {\n");
	printf("\tpre : ");
	for(int i=0; i<7; i++)
		printf("%x ",head->pre[i]);
	printf("\n\tsfd : %x\n",head->sfd);
	printf("\tdst mac : ");
	print_mac_addr(head->dst_addr);
	printf("\n\tsrc mac : ");
	print_mac_addr(head->src_addr);
	printf("\n");
	if ( mac_has_tag(head)){
		printf("\ttpid : %x\n",head->tpid);
		printf("\ttci  : %x\n",head->tci);
	}
	printf("\ttype : %x\n}\n",head->type);
}

void print_mac_foot(mac_foot_s * foot){
	printf("mac foot {\n");
	printf("\tcrc : %x\n}\n",foot->crc);

}


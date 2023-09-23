#include "mac.h"
#include "eth_defs.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
uint8_t *write_mac_head(mac_head_s *head, size_t *len){
	assert(head);

	/* calculate mac header length */
	*len = get_mac_head_len(head);
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));

	/* copy head up until tag/type */
	memcpy(buff, head, MAC_HEAD_BASE_LITE_SIZE);
	if(mac_has_tag(head)){
		/* has tag */
		memcpy(buff+MAC_HEAD_BASE_LITE_SIZE,
			((uint8_t*)head)+MAC_HEAD_BASE_LITE_SIZE,
			6);	
	}else{
		/* doesn't have tag, only has type */
		memcpy(buff+MAC_HEAD_BASE_LITE_SIZE,
			&(head->type),
			2);
	}
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
	*len = MAC_FOOT_SIZE;
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memcpy( buff, foot, sizeof(uint8_t)*(*len));
	return buff;
}

/* Calculate crc */
uint32_t calculate_crc(uint8_t *buff, size_t len){
	uint32_t crc = 0xffffffff;
	uint32_t b; 
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
	}
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
		mac->tpid = 0x8100;
		mac->tci = 0;
	}else{
		mac->tpid = 0;
	}
	mac->type = 0x08; /* IPv4 */
	
	print_mac_head(mac);

	#ifdef DEBUG
	print_mac_head(mac);
	#endif

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
		printf("\ttpid : %x\n",head->tpid);
	}
	printf("\ttype : %x\n}\n",head->type);
}

void print_mac_foot(mac_foot_s * foot){
	printf("mac foot {\n");
	printf("\tcrc : %x\n}\n",foot->crc);

}


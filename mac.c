#include "mac.h"
#include "eth_defs.h"
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


#include "ipv4.h"
#include "eth_defs.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _BSD_SOURCE
#include <endian.h>

/* IPv4 */

ipv4_head_s *read_ipv4_head(uint8_t *buff, size_t len){
	assert(buff);
	assert(len >= IP_HEAD_BASE_SIZE ); /* minumum lenght for ipv4 header */

	ipv4_head_s * head = malloc(sizeof(ipv4_head_s));

	/* copy at a bit level */
	head->ver = (uint8_t)0xf & buff[0];
	head->ihl = (uint8_t)0xf & ( buff[0] >> 4 );
	head->dscp = (uint8_t)0x3f & buff[1];
	head->ecn = (uint8_t)0x3 & ( buff[1] >> 6 );
	memcpy(&(head->tot_len), buff+2, sizeof(head->tot_len)); 
	memcpy(&(head->id), buff+4, sizeof(head->id));
	head->flags = (uint8_t)0x7 & buff[6];
	head->frag_off = ( (uint8_t)0x3f & (buff[6]>>3)) | ( (uint16_t)buff[7] << 8 );
	head->ttl = buff[8];
	head->prot = buff[9];
	memcpy(&(head->head_cs), buff+10, sizeof(head->head_cs));
	memcpy(&(head->src_addr), buff+12, sizeof(head->src_addr));
	memcpy(&(head->dst_addr), buff+16, sizeof(head->dst_addr));

	return head;
}
uint8_t *write_ipv4_head(ipv4_head_s *head, size_t *len){
	assert(head);
	*len = IP_HEAD_BASE_SIZE;
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memset(buff, 0, *len);

	/* copy at a bit level */
	buff[0] |= (uint8_t)0xf & head->ihl;	
	buff[0] |= (uint8_t)0xf0 & ((uint8_t)head->ver << 4);	
	buff[1] |= (uint8_t)0xfc & ((uint8_t)head->dscp<<2);	
	buff[1] |= (uint8_t)0x03 & head->ecn;

	uint16_t be_tot_len = htobe16(head->tot_len);
	memcpy(buff+2, &be_tot_len, 2);	

	uint16_t be_id = htobe16(head->id);
	memcpy(buff+4, &be_id, 2);

	buff[6] = (uint8_t)0e0 & ((uint8_t)head->flags << 5);
	buff[6] = (uint8_t)0x1f & ((uint8_t)head->frag_off);
	buff[7] = (uint8_t)0xff & ((uint32_t)(head->frag_off<<5));
	buff[8] = head->ttl;
	buff[9] = head->prot;

	uint16_t be_cs = htobe16(head->head_cs);
	memcpy(buff+10, &be_cs, 2);	

	uint32_t be_src_addr = htobe32(head->src_addr);
	memcpy(buff+12, &be_src_addr, 4);	
	
	uint32_t be_dst_addr = htobe32(head->dst_addr);
	memcpy(buff+16, &be_dst_addr, 4);

	return buff;	
}

/* get protocol type */
bool ipv4_protocol_is_udp(ipv4_head_s *head){
	assert(head);
	return head->prot == PROTOCOL_UDP;
}

ipv4_head_s *init_ipv4_head(
	const uint32_t src_addr,
	const uint32_t dst_addr,
	const size_t ip_data_len,
	const uint8_t protocol
){
	ipv4_head_s *head = malloc(sizeof(ipv4_head_s));
	head->ver = 4;
	head->ihl = 5;
	head->dscp = 0;
	head->ecn = 0;
	head->tot_len = (uint16_t)(((uint16_t)(head->ihl))*5) + (uint16_t)ip_data_len;
	head->id = 0;
	head->flags = 1;
	head->frag_off = 0;
	head->ttl = 64;
	head->prot = protocol;
	head->src_addr = src_addr;
	head->dst_addr = dst_addr;
	head->head_cs = calculate_ipv4_header_checksum(head);

	return head;
}

uint16_t calculate_ipv4_header_checksum(ipv4_head_s *head){
	uint16_t sum = 0;
	sum += (uint16_t)((uint16_t)(head->ecn)  << 14 )
		 | ((uint16_t)(head->dscp) << 8 )
		 | ((uint16_t)(head->ihl)  << 4 )
		 |  (uint16_t)(head->ver);
	sum += head->tot_len;
	sum += head->id;
	sum += (uint16_t)(head->flags)
		 | ((uint16_t)(head->frag_off) << 2);
	sum += (uint16_t)(head->ttl)
		 | ((uint16_t)(head->prot)<<8);
	sum += (uint16_t)(head->src_addr);	
	sum += (uint16_t)((uint16_t)(head->src_addr)>>16);	
	sum += (uint16_t)(head->dst_addr);	
	sum += (uint16_t)(head->dst_addr>>16);
	return sum;	
}

void print_ipv4_head(ipv4_head_s *head){
	printf("ip head {\n");
	printf("\tver : %x\n",head->ver);
	printf("\tihl : %x\n",head->ihl);
	printf("\tdscp : %x\n",head->dscp);
	printf("\tecn : %x\n",head->ecn);
	printf("\ttot len : %u\n",head->tot_len);
	printf("\tid : %u\n",head->id);
	printf("\tflags : %x\n",head->flags);
	printf("\tflag off : %u\n",head->frag_off);
	printf("\tttl : %u\n",head->ttl);
	printf("\tprot : %u\n",head->prot);
	printf("\thead cs : %x\n",head->head_cs);
	printf("\tsrc addr : ");
	print_ipv4_addr(head->src_addr);
	printf("\n");
	printf("\tdst addr : ");
	print_ipv4_addr(head->dst_addr);
	printf("\n}\n");
}


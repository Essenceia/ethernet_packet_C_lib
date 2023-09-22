#include "eth_packet_s.h"
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
	buff[0] |= (uint8_t)0xf & head->ver;	
	buff[0] |= (uint8_t)0xf0 & ((uint8_t)head->ihl << 4);	
	buff[1] |= (uint8_t)0x3f & head->dscp;	
	buff[1] |= (uint8_t)0xc0 & ((uint8_t)head->ecn << 6);
	memcpy(buff+2, &(head->tot_len), 2);	
	memcpy(buff+4, &(head->id), 2);
	buff[6] = (uint8_t)0x7 & ((uint8_t)head->flags);
	buff[6] = (uint8_t)0xf8 & ((uint16_t)head->frag_off<<3);
	buff[7] = (uint8_t)0xff & ((uint32_t)head->frag_off<<11);
	buff[8] = head->ttl;
	buff[9] = head->prot;
	memcpy(buff+10, &(head->head_cs), 2);	
	memcpy(buff+12, &(head->src_addr), 4);	
	memcpy(buff+16, &(head->dst_addr), 4);

	return buff;	
}

/* get protocol type */
bool ipv4_protocol_is_udp(ipv4_head_s *head){
	assert(head);
	return head->prot == PROTOCOL_UDP;
}

/* UDP */
udp_head_s *read_udp_head(uint8_t *buff, size_t len){
	assert(buff);
	assert(len>= UDP_HEAD_SIZE);
	
	udp_head_s * head = malloc(sizeof(udp_head_s));
	memcpy(head, buff, sizeof(udp_head_s) );
	
	return head;
}
uint8_t *write_udp_head(udp_head_s* head, size_t *len){
	assert(head);
	*len = UDP_HEAD_SIZE;
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memcpy(buff, head, (*len));
	return buff;	
} 


/* Full packet */
/* read buffer data into ethernet packet */
eth_packet_s * read_eth_packet(uint8_t *buff, size_t len){
	assert(buff);
	assert(len >= MAC_HEAD_BASE_SIZE+IP_HEAD_BASE_SIZE+UDP_HEAD_SIZE);

	eth_packet_s *pkt = malloc(sizeof(eth_packet_s));
	size_t off = 0;
	/* read header
	 * mac */
	pkt->mac_head = read_mac_head(buff, len);
	off = get_mac_head_len(pkt->mac_head);

	/* ipv4 */
	pkt->ipv4_head = read_ipv4_head(buff+off, len-off);
	off += IP_HEAD_BASE_SIZE;
	
	/* transport protocol */
	switch(pkt->ipv4_head->prot){
		case PROTOCOL_UDP :
			pkt->udp_head = read_udp_head(buff+off, len-off);
			off += UDP_HEAD_SIZE;
			break;
		default :
			printf("ERROR: Unexpected transport protocol, got %x\n", pkt->ipv4_head->prot);
			assert(0);
			break;
	}
	/* copy application data */
	pkt->data_len = len - off - MAC_FOOT_SIZE;
	memcpy(pkt->data, buff+off, pkt->data_len);
	off += pkt->data_len;

	/* footer */
	pkt->mac_foot = read_mac_foot(buff+off,len-off);

	return pkt; 
}

uint8_t *write_eth_packet(eth_packet_s* pkt, size_t *len){
	assert(pkt);
	
	/* head */
	uint8_t *mac_head_buff;	
	uint8_t *ipv4_head_buff;	
	uint8_t *trans_head_buff;	
	size_t mac_len, ipv4_len, trans_len;
	mac_head_buff = write_mac_head(pkt->mac_head, &mac_len);
	ipv4_head_buff = write_ipv4_head(pkt->ipv4_head, &ipv4_len);
	switch (pkt->ipv4_head->prot){
		case PROTOCOL_UDP:
			trans_head_buff = write_udp_head(pkt->udp_head, &trans_len);
			break;
		default :
			printf("ERROR: Unexpected transport protocol, got %x\n", pkt->ipv4_head->prot);
			assert(0);
			break;	
	}
	/* foot */
	uint8_t *mac_foot_buff;
	size_t mac_foot_len;
	mac_foot_buff = write_mac_foot(pkt->mac_foot, &mac_foot_len);

	/* calculate total length */
	(*len) = mac_len + ipv4_len + trans_len + pkt->data_len + mac_foot_len;
	uint8_t *buff = malloc(sizeof(uint8_t)*(*len));
	memcpy(buff, mac_head_buff, mac_len);
	size_t off = mac_len;
	memcpy(buff+off, ipv4_head_buff, ipv4_len);
	off += ipv4_len;
	memcpy(buff+off, trans_head_buff, trans_len);
	off+= trans_len;
	memcpy(buff+off, pkt->data, pkt->data_len);
	off += pkt->data_len;
	memcpy(buff+off, mac_foot_buff, mac_foot_len);
	off += mac_foot_len;

	assert( off == (*len));

	return buff;				
}


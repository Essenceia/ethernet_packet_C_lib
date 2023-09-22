#include "eth_packet_s.h"
#include "eth_defs.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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


/* Copyright (c) 2023, Julia Desmazes. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial
 * 4.0 International License.
 *
 * This code is provided "as is" without any express or implied warranties. */

#include "eth.h"
#include "defs.h"
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
	size_t buff_len = mac_len + ipv4_len + trans_len + pkt->data_len + mac_foot_len;
	(*len) = buff_len;
	info("eth packet buff len %ld\n", *len);

	uint8_t *buff = malloc(sizeof(uint8_t)*buff_len);

	memcpy(buff, mac_head_buff, mac_len);
	#ifdef DEBUG
	info("mac head buffer : \n");
	for(int s=0; s<mac_len;s++){
		info("[%d] %x\n",s,buff[s]);
	}
	#endif

	size_t off = mac_len;
	memcpy(buff+off, ipv4_head_buff, ipv4_len);
	#ifdef DEBUG
	info("ipv4 head buffer : \n");
	for(int s=off; s<off+ipv4_len;s++){
		info("[%d] %x\n",s,buff[s]);
	}
	#endif

	off += ipv4_len;
	memcpy(buff+off, trans_head_buff, trans_len);
	#ifdef DEBUG
	info("udp head buffer : \n");
	for(int s=off; s<off+trans_len;s++){
		info("[%d] %x\n",s,buff[s]);
	}
	#endif

	off+= trans_len;
	memcpy(buff+off, pkt->data, pkt->data_len);
	#ifdef DEBUG
	info("data buffer : \n");
	for(int s=off; s<off+pkt->data_len;s++){
		info("[%d] %x\n",s,buff[s]);
	}
	#endif

	off += pkt->data_len;
	memcpy(buff+off, mac_foot_buff, mac_foot_len);
	#ifdef DEBUG
	info("mac foot buffer : \n");
	for(int s=off; s<off+mac_foot_len;s++){
		info("[%d] %x\n",s,buff[s]);
	}
	#endif
	off += mac_foot_len;

	free(mac_head_buff);
	free(ipv4_head_buff);
	free(trans_head_buff);
	free(mac_foot_buff);

	assert( off == (*len));

	#ifdef DEBUG
	info("buff len %ld\n", (*len));
	for(int s=0; s<(*len); s++){
		info("[%ld] %x\n",s,buff[s]);
	}
	#endif

	return buff;				
}

eth_packet_s *init_eth_packet(
	const uint8_t dst_mac[6],
	const uint8_t src_mac[6],
	const uint32_t src_ip,
	const uint32_t dst_ip,
	const uint16_t src_port,
	const uint16_t dst_port, 
	const bool vtag
){
	info("Init eth packet");

	eth_packet_s *eth = malloc(sizeof(eth_packet_s));

	/* set data len to 0 */
	eth->data_len = 0;
	eth->data = NULL;

	/* MAC head */
	eth->mac_head = init_mac_head(
		dst_mac, 
		src_mac, 
		vtag);

	/* IPv4 head */
	size_t ip_data_len =eth->data_len + UDP_HEAD_SIZE;
	eth->ipv4_head = init_ipv4_head(src_ip,
									dst_ip,	
									ip_data_len,	
									PROTOCOL_UDP);
	/* TCP head uninitialized by default */
	eth->tcp_head = NULL;
	
	/* UDP head */
	eth->udp_head = init_udp_head(src_port,
								  dst_port,
								  (uint16_t)eth->data_len);
	/* MAC foot */
	eth->mac_foot = init_mac_foot();

	update_eth_packet_crc(eth);

	return eth;
}
/* update application data */
void update_eth_packet_data(
	eth_packet_s *eth, 
	uint8_t *app_data, 
	size_t app_data_len)
{
	assert(eth);
	
	/* copy data */
	if(app_data_len){
		assert(app_data);
		if (!eth->data){
			/* allocate if data has never been allocated */
			eth->data = malloc(app_data_len);
		}else{
			eth->data = realloc(eth->data, app_data_len);
		}
		memcpy(eth->data, app_data, app_data_len);
	}
	eth->data_len = app_data_len;

	/* update udp 
 	 * modify data len, cs is optional and unused */
	set_udp_len(eth->udp_head, eth->data_len);
	/* update ipv4 */
	size_t payload_len = eth->data_len + UDP_HEAD_SIZE;
	update_ipv4_header_data_len(eth->ipv4_head, payload_len);
	/* update mac */
	update_eth_packet_crc(eth);	
}


void update_eth_packet_crc(eth_packet_s *eth){
	/* translate packet into contents into uint8_t buffer */
	uint8_t *buff;
	size_t buff_len;
	buff = write_eth_packet(eth, &buff_len);
	/* skip mac preamble */
	buff_len -= 8 + 4;
	uint32_t crc = calculate_crc(buff+8, buff_len);
	eth->mac_foot->crc = crc;
	free(buff);	
}

void free_eth_packet(eth_packet_s* eth){
	assert(eth);
	free(eth->mac_head);
	free(eth->ipv4_head);
	if(eth->udp_head != NULL)
		free(eth->udp_head);
	if(eth->tcp_head != NULL)
		free(eth->tcp_head);
	free(eth->data);
	free(eth->mac_foot);
	free(eth);
}

void print_eth_packet(eth_packet_s * eth){
	assert(eth);
	/* heads */
	print_mac_head(eth->mac_head);
	print_ipv4_head(eth->ipv4_head);
	print_udp_head(eth->udp_head);
	/* application data */
	printf("app {\n\tlen : %ld\n\tdata :\n", eth->data_len);
	for(size_t i=0; i< eth->data_len; i++){
		printf("\t[%ld] : %x\n",i, eth->data[i]);
	}
	printf("}\n");
	/* footer */
	print_mac_foot(eth->mac_foot);
}
bool is_udp(eth_packet_s *eth){
	assert(eth);
	return (eth->ipv4_head->prot == PROTOCOL_UDP);
}

size_t get_head_len(eth_packet_s *eth){
	assert(eth);
	size_t len;
	/* get packet head len */
	len = get_mac_head_len(eth->mac_head)
		+ get_ipv4_head_len(eth->ipv4_head);
	if(is_udp(eth)){
		len += UDP_HEAD_SIZE; 
	}else{
		/* TODO: add support for tcp */
		assert(0);
	}
	return len;
}


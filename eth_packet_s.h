#ifndef ETH_PACKET_S_H
#define ETH_PACKET_S_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct{
	uint8_t pre[7];
	uint8_t sfd; /* start of frame delimiter */
	uint8_t dst_addr[6];
	uint8_t src_addr[6];
	/* tag */
	uint16_t tpid;
	uint16_t tci; /* tag control information */
	/* type */
	uint16_t type;
}__attribute__((__packed__)) mac_head_s;

mac_head_s *read_mac_head(uint8_t *buff, size_t len);

uint8_t *write_mac_head(mac_head_s *head, size_t *len);

bool mac_has_tag(mac_head_s *mac_head);

size_t get_mac_head_len(mac_head_s *head);

typedef struct{
	uint8_t crc[4];
}__attribute__((__packed__)) mac_foot_s;

mac_foot_s *read_mac_foot(uint8_t *buff, size_t len);
uint8_t *write_mac_foot(mac_foot_s*foot, size_t *len);
/* ipv4 */
typedef struct{
	uint8_t ver : 4;/* 4 bits */
	uint8_t ihl : 4;
	uint8_t dscp : 6;
	uint8_t ecn : 2;
	uint16_t tot_len;
	uint16_t id;
	uint8_t flags : 3;
	uint16_t frag_off : 13;
	uint8_t ttl;
	uint8_t prot;
	uint16_t head_cs;
	uint64_t src_addr;
	uint64_t dst_addr;
}__attribute__((__packed__)) ipv4_head_s;

ipv4_head_s *read_ipv4_head(uint8_t *buff, size_t len);
uint8_t *write_ipv4_head(ipv4_head_s *head, size_t *len);

bool ipv4_protocol_is_udp(ipv4_head_s *head);

/* udp */
typedef struct{
	uint32_t src_port;
	uint32_t dst_port;
	uint32_t len;
	uint32_t cs;
}udp_head_s;

udp_head_s *read_udp_head(uint8_t *buff, size_t len);
uint8_t *write_udp_head(udp_head_s* head, size_t *len); 
/* tcp */
typedef struct{

}tcp_head_s;

/* udp/tcp packet */
typedef struct{
	/* head */
	mac_head_s  *mac_head;
	ipv4_head_s *ipv4_head;
	udp_head_s  *udp_head;
	tcp_head_s  *tcp_head;
	/* data */
	uint8_t *data;
	size_t data_len;
	/* footer */
	mac_foot_s *mac_foot;
}eth_packet_s;

bool is_udp(eth_packet_s *eth);

eth_packet_s * read_eth_packet(uint8_t *buff, size_t len);
uint8_t *write_eth_packet(eth_packet_s* eth, size_t *len);

#endif //ETH_PACKET_S_H

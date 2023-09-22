#ifndef ETH_PACKET_S_H
#define ETH_PACKET_S_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "mac.h"
#include "ipv4.h"

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

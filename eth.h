#ifndef ETH_H
#define ETH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "mac.h"
#include "ipv4.h"
#include "udp.h"
#include "tcp.h"

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


#endif //ETH_S_H

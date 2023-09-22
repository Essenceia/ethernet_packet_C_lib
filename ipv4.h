#ifndef IPV4_H
#define IPV4_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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

/* constructor */
ipv4_head_s *init_ipv4_head(
	const uint64_t src_addr,
	const uint64_t dst_addr,
	const size_t ip_data_len,
	const uint8_t protocol
);
/* calculate header checksum */
uint16_t calculate_ipv4_header_checksum(ipv4_head_s *head);
#endif //IPV4_H

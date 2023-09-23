#ifndef UDP_H 
#define UDP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* udp */
typedef struct{
	uint32_t src_port;
	uint32_t dst_port;
	uint32_t len;
	uint32_t cs;
}udp_head_s;

udp_head_s *read_udp_head(uint8_t *buff, size_t len);

void set_udp_len(udp_head_s *head, size_t data_len);

uint8_t *write_udp_head(udp_head_s* head, size_t *len); 

udp_head_s * init_udp_head(
	const uint32_t scr_port,
	const uint32_t dst_port,
	const uint32_t udp_data_len);	

void print_udp_head(udp_head_s * head);

#endif // UDP_H

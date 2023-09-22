#ifndef MAC_H
#define MAC_H
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

uint32_t calculate_crc(uint8_t *buff, size_t len);
#endif //MAC_H

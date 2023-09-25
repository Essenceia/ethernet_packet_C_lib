#ifndef TCP_H
#define TCP_H

/* tcp */
typedef struct{
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint8_t data_off; /* 4 bits */
	const uint8_t res = 0; /* 4 bits : reserved 0000 */
	bool cwr;
	bool ece;
	bool urg;
	bool ack;
	bool psh;
	bool rst;
	bool syn;
	bool fin;
	uint16_t win_size; 
	uint16_t cs;
	uint16_t urg_ptr; 
}__attribute__((__packed__)) tcp_head_s;

tcp_head_s *read_tcp_head(uint8_t *buff, size_t len);

uint8_t *write_tcp_head(tcp_head_s *head, size_t *len);

#endif //TCP_H

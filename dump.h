#ifndef DUMP_H
#define DUMP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void dump_eth_packet(
	const uint8_t *buff,
	const size_t len,
	const bool inbound
);
#endif //DUMP_H

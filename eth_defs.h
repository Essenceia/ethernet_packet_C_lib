#ifndef ETH_DEFS_H
#define ETH_DEFS_H
#include <stdint.h>

#define MAC_TPID (uint16_t) 0x8100

/* IP protocol numbers */
#define PROTOCOL_UDP (uint8_t) 0x11
#define PROTOCOL_TCP (uint8_t) 0x06

/* Packets size in bytes */
#define MAC_HEAD_BASE_LITE_SIZE (size_t) 20
#define MAC_HEAD_BASE_SIZE (size_t) 22
#define MAC_HEAD_TAG_SIZE (size_t) 4
#define MAC_FOOT_SIZE (size_t) 4

/* doesn't support IP options */
#define IP_HEAD_BASE_SIZE (size_t) 20

#define UDP_HEAD_SIZE (size_t) 8

/* default values */
#define DEFAULT_DST_MAC ((uint8_t[6]) { 0xFC, 0xFC, 0x48, 0, 0, 0})
#define DEFAULT_SRC_MAC ((uint8_t[6]) { 0x00, 0x00, 0x01, 0, 0, 0})
#define DEFAULT_DST_IP (uint32_t) 0x0
#define DEFAULT_SRC_IP (uint32_t) 0x1
#define DEFAULT_DST_PORT (uint32_t) 80
#define DEFAULT_SRC_PORT (uint32_t) 80
#endif // ETH_DEFS_H

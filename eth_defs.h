#ifndef ETH_DEFS_H
#define ETH_DEFS_H

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

#endif // ETH_DEFS_H

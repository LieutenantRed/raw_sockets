#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

struct {
	unsigned char ihl:4, ver:4; 
	unsigned char tos;
	uint16_t len;
	uint16_t id;
	uint16_t flg_off;
	unsigned char ttl;
	unsigned char proto;
	uint16_t csum;
	uint32_t ip_src;
	uint32_t ip_dst;
} ip_head;

struct {
	uint16_t src;
	uint16_t dst;
	uint16_t len;
	uint16_t csum;
} udp_head;

struct {
	u_char	ether_dhost[6];
	u_char	ether_shost[6];
	u_short	ether_type;
} eth_head;

uint16_t ip_checksum(void* ip_p);

#endif
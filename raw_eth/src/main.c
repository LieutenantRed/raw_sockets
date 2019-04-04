#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#include "components.h"

#define PROTO_UDP 17

#define BUFFER_SIZE 500
#define HOST "192.168.1.2"
#define HOST_PORT 2007
#define THIS "192.168.1.3"
#define THIS_PORT 1952

int main() {
	int raw_udp = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (raw_udp == -1) {
		fprintf(stderr, "ETH socket error");
		exit(EXIT_FAILURE);
	}

	/*ETHERNET HEADER*/
	// dest:  e0:b9:a5:ef:fa:ea
	// this:  0c:8b:fd:5d:0f:52
	memset(&eth_head, 0, sizeof(eth_head));

	eth_head.ether_dhost[0] = 0xe0;
	eth_head.ether_dhost[1] = 0xb9;
	eth_head.ether_dhost[2] = 0xa5;
	eth_head.ether_dhost[3] = 0xef;
	eth_head.ether_dhost[4] = 0xfa;
	eth_head.ether_dhost[5] = 0xea;

	eth_head.ether_shost[0] = 0x0c;
	eth_head.ether_shost[1] = 0x8b;
	eth_head.ether_shost[2] = 0xfd;
	eth_head.ether_shost[3] = 0x5d;
	eth_head.ether_shost[4] = 0x0f;
	eth_head.ether_shost[5] = 0x52;

	eth_head.ether_type = 0x0008;

	/*IP HEADER*/
	memset(&ip_head, 0, sizeof(ip_head));

	ip_head.ihl = 5;
	ip_head.ver = 4;
	ip_head.len = htons(sizeof(ip_head) + sizeof(udp_head) + BUFFER_SIZE);
	ip_head.ttl = 255;
	ip_head.proto = PROTO_UDP;
	inet_aton(THIS, (struct in_addr*)&(ip_head.ip_src));
	inet_aton(HOST, (struct in_addr*)&(ip_head.ip_dst));

	/*UDP HEADER*/
	memset(&udp_head, 0, sizeof(udp_head));

	udp_head.src = htons(THIS_PORT);
	udp_head.dst = htons(HOST_PORT);
	udp_head.len = htons(BUFFER_SIZE);
	udp_head.csum = 0;


	/*MSG BUFFER*/
	int full_l = BUFFER_SIZE +
		sizeof(udp_head) +
		sizeof(ip_head) +
		sizeof(eth_head) + 4;

	char buffer[full_l];
	memset(buffer, 0, full_l);

	memcpy(
		buffer,
		&eth_head,
		sizeof(eth_head)
	);
	memcpy(
		(char *)buffer + sizeof(eth_head),
		&ip_head,
		sizeof(ip_head)
	);
	memcpy(
		(char *)buffer + sizeof(ip_head) + sizeof(eth_head),
		&udp_head,
		sizeof(udp_head)
	);

	char *buffer_it = (char *)buffer + sizeof(ip_head) + sizeof(eth_head) + sizeof(udp_head);

	struct sockaddr_ll p_addr;
	memset(&p_addr, 0, sizeof(p_addr));
	p_addr.sll_family = AF_PACKET;
	p_addr.sll_halen = 6;
	p_addr.sll_ifindex = if_nametoindex("wlp2s0");

	/*BINDING*/
	// if (bind(raw_udp, (struct sockaddr *)&p_addr, sizeof(p_addr)) != 0) {
	// 	fprintf(stderr, "binding socket error");
	// 	exit(EXIT_FAILURE);
	// }

	/*PUSH MSG*/
	while (1) {
		void *csum_p = (void *) &(ip_head.csum);
		memset(csum_p, 0, sizeof(uint16_t));

		uint16_t current_csum = ip_checksum(&ip_head);
		memcpy(csum_p, &current_csum, sizeof(uint16_t));
		memcpy(
			(char *)buffer + sizeof(eth_head),
			&ip_head,
			sizeof(ip_head)
		);
		//clear
		memset(buffer_it, 0, BUFFER_SIZE);

		//send
		fgets(buffer_it, BUFFER_SIZE, stdin);
		uint32_t csum = crc32((unsigned char*)buffer, full_l - 4);
		memcpy(buffer + full_l - 4, &csum, 4);

		sendto(raw_udp, buffer, full_l, 0, (struct sockaddr*)&p_addr, sizeof(p_addr));

		if (strcmp(buffer_it, "exit\n") == 0) {
			break;
		}

		//recv
		char printbuf[full_l];
		memset(printbuf, 0, full_l);
		void *dataptr, *rcv_ipptr, *rcv_portptr;

		dataptr = printbuf + sizeof(eth_head) + sizeof(ip_head) + sizeof(udp_head);
		rcv_ipptr = printbuf + sizeof(eth_head) + sizeof(ip_head) - sizeof(uint32_t);
		rcv_portptr = printbuf + sizeof(eth_head) + sizeof(ip_head) + sizeof(uint16_t);
		
		while (ntohs(*((uint16_t *)rcv_portptr)) != THIS_PORT) {
			recvfrom(raw_udp, printbuf, full_l, 0, 0, 0);
		}
		printf("%s", (char*)dataptr);
	}

	close(raw_udp);

}

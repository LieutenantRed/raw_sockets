#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define PROTO_UDP 17

#define BUFFER_SIZE 1024
#define HOST "192.168.1.7"
#define HOST_PORT 2007
#define THIS "192.168.1.3"
#define THIS_PORT 1952

int main() {
	int raw_udp = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (raw_udp == -1) {
		fprintf(stderr, "UDP socket error");
		exit(EXIT_FAILURE);
	}

	int ip_opt_enabled = 1;
	setsockopt(raw_udp, IPPROTO_IP, IP_HDRINCL, &ip_opt_enabled, sizeof(int));
	
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

	/*IP HEADER*/
	memset(&ip_head, 0, sizeof(ip_head));

	ip_head.ihl = 5;
	ip_head.ver = 4;
	ip_head.len = htons(sizeof(ip_head) + sizeof(udp_head));
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
	char buffer[BUFFER_SIZE + sizeof(udp_head) + sizeof(ip_head)];
	memset(buffer, 0, BUFFER_SIZE + sizeof(udp_head) + sizeof(ip_head));

	memcpy(buffer, &ip_head, sizeof(ip_head));
	memcpy((char*)buffer + sizeof(ip_head), &udp_head, sizeof(udp_head));

	struct sockaddr_in host_addr;
	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(HOST_PORT);
	inet_aton(HOST, (struct in_addr*)&(host_addr.sin_addr));

	/*PUSH MSG*/
	while (1) {
		//clear
		memset(buffer + sizeof(udp_head) + sizeof(ip_head), 0, BUFFER_SIZE);

		//send
		fgets(buffer + sizeof(udp_head) + sizeof(ip_head), BUFFER_SIZE, stdin);

		sendto(
			raw_udp, 
			buffer, 
			BUFFER_SIZE + sizeof(udp_head) + sizeof(ip_head), 
			0, 
			(struct socksaddr*)&host_addr, 
			sizeof(host_addr)
		);

		if (strcmp(buffer + sizeof(udp_head) + sizeof(ip_head), "exit\n") == 0) {
			break;
		}

		//recv
		char printbuf[BUFFER_SIZE];
		memset(printbuf, 0, BUFFER_SIZE);
		memset(&udp_head, 0, sizeof(udp_head));
		while (ntohs(udp_head.dst) != THIS_PORT) {
			recvfrom(raw_udp, printbuf, BUFFER_SIZE, 0, 0, 0);
			memcpy(&udp_head, printbuf + 20, sizeof(udp_head));
		}
		printf("%s", printbuf + 20 + 8);
	}

	close(raw_udp);

}
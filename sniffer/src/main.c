#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#define BUFFER_SIZE 1024

int main() {
	int raw_udp = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (raw_udp == -1) {
		fprintf(stderr, "UDP socket error");
		exit(EXIT_FAILURE);
	}

	int raw_tcp = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (raw_tcp == -1) {
		fprintf(stderr, "TCP socket error");
		exit(EXIT_FAILURE);
	}

	fd_set socketset, workset;
	FD_ZERO(&socketset);
	FD_SET(raw_tcp, &socketset);
	FD_SET(raw_udp, &socketset);

	struct sockaddr_in src;
	int addrlen = sizeof(src);
	char buffer[BUFFER_SIZE];
	struct iphdr header;
	struct tcphdr tcp_head; //size = u16 * 6 + u32 * 2
	struct udphdr udp_head; //size = u16 * 4

	while(1) {
		memcpy(&workset, &socketset, sizeof(fd_set));
		select(FD_SETSIZE, &workset, NULL, NULL, NULL);
		
		if (FD_ISSET(raw_tcp, &workset)) {
			memset(buffer, 0, BUFFER_SIZE);
			recvfrom(raw_tcp, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src, (socklen_t*)&addrlen);
			
			// Without parsing:
			// for (int i = 0; i < 1024; ++i)
			// 	fprintf(stdout, "%c", buffer[i]);
			// fprintf(stdout, "\n\n");

			//Output
			fprintf(stdout, "TCP: from %s : %d\n", inet_ntoa(src.sin_addr), ntohs(src.sin_port));
			for (int i = sizeof(header) + sizeof(tcp_head); i < BUFFER_SIZE; ++i) {
				fprintf(stdout, "%c ", buffer[i]);
			}
			fprintf(stdout, "\n\n");
		}

		if (FD_ISSET(raw_udp, &workset)) {
			memset(buffer, 0, BUFFER_SIZE);
			recvfrom(raw_udp, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src, (socklen_t*)&addrlen);
			//Output
			fprintf(stdout, "UDP: from %s : %d\n", inet_ntoa(src.sin_addr), ntohs(src.sin_port));
			for (int i = sizeof(header) + sizeof(udp_head); i < BUFFER_SIZE; ++i) {
				fprintf(stdout, "%c ", buffer[i]);
			}
			fprintf(stdout, "\n\n");
		}
	}

	return 0;
} 
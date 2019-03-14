#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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

	while(1) {
		memcpy(&workset, &socketset, sizeof(fd_set));
		select(FD_SETSIZE, &workset, NULL, NULL, NULL);
		
		if (FD_ISSET(raw_tcp, &workset)) {
			memset(buffer, 0, BUFFER_SIZE);
			recvfrom(raw_tcp, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src, (socklen_t*)&addrlen);
			fprintf(stdout, "TCP: from %s : %d\n%s\n\n", inet_ntoa(src.sin_addr), ntohs(src.sin_port), buffer);
		}

		if (FD_ISSET(raw_udp, &workset)) {
			memset(buffer, 0, BUFFER_SIZE);
			recvfrom(raw_udp, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src, (socklen_t*)&addrlen);
			fprintf(stdout, "UDP: from %s : %d\n%s\n\n", inet_ntoa(src.sin_addr), ntohs(src.sin_port), buffer);
		}
	}

	return 0;
} 
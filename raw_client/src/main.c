#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/udp.h>

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

	struct sockaddr_in host_addr;
	memset(&host_addr, 0, sizeof(host_addr));
	
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(HOST_PORT);
	inet_aton(HOST, &host_addr.sin_addr);

	struct {
		uint16_t src;
		uint16_t dst;
		uint16_t len;
		uint16_t csum;
	} head;
	memset(&head, 0, sizeof(head));

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	head.src = htons(THIS_PORT);
	head.dst = htons(HOST_PORT);
	head.len = htons(BUFFER_SIZE);
	head.csum = 0;

	memcpy(&buffer, &head, sizeof(head));

	
	while (1) {
		//clear
		memset(buffer + sizeof(head), 0, BUFFER_SIZE - sizeof(head));

		//send
		fgets(buffer + sizeof(head), BUFFER_SIZE - sizeof(head), stdin);

		int len = sizeof(host_addr);
		sendto(raw_udp, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&host_addr, len);
		if (strcmp(buffer + sizeof(head), "exit\n") == 0) {
			break;
		}

		//recv
		char printbuf[BUFFER_SIZE];
		memset(printbuf, 0, BUFFER_SIZE);
		memset(&head, 0, sizeof(head));
		while (ntohs(head.dst) != THIS_PORT) {
			recvfrom(raw_udp, printbuf, BUFFER_SIZE, 0, 0, 0);
			memcpy(&head, printbuf + 20, sizeof(head));
		}
		printf("%s", printbuf + 20 + 8);
	}

	close(raw_udp);

}
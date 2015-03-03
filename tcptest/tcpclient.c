#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main() {

	int 	sockfd;
	struct	addrinfo hints, *res;
	char	buf[100] = {0};

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		perror("Cannot create socket\n");
		return 1;
	}

	memset((char *)&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo("128.10.3.120", "12345", &hints, &res) == -1) {
		perror("getaddrinfo ");
		return 1;
	}

	if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("connect");
		return 1;
	}
	printf("connected!\n");

	char	name[50];
	char	msg[20];

	printf("Enter your name: ");
	scanf("%s", name);

	send(sockfd, name, 10, 0);
	recv(sockfd, msg, 20, 0);
	msg[19] = 0;
	printf("%s\n", msg);

	close(sockfd);

	return 0;
}

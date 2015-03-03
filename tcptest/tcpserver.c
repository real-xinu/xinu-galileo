#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main() {

	int	sockfd;
	struct	addrinfo hints, *res;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		perror("socket");
		return 1;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(NULL, "12348", &hints, &res) == -1) {
		perror("getaddrinfo");
		return 1;
	}

	if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("bind");
		return 1;
	}

	if(listen(sockfd, 5) == -1) {
		perror("listen");
		return 1;
	}

	while(1) {
		struct sockaddr_in addr;
		int addrlen = sizeof(addr);

		int new = accept(sockfd, &addr, &addrlen);
		printf("new = %d\n", new);

		char name[20]={0};
		memset(name, 0, 20);
		recv(new, name, 5, 0);
		printf("Name received: %s\n", name);
		char msg[50];
		sprintf(msg, "Hello %s\n", name);
		send(new, msg, strlen(msg), 0);
		close(new);
	}

	close(sockfd);
	return 0;
}

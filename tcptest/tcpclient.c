#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

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
/*	recv(sockfd, buf, 100, 0);
	printf("Read from the server: %s\n", buf);
	while(1) {
		if(recv(sockfd, buf, 100, 0) == 0) {
			close(sockfd);
			break;
		}
	}*/
	//send(sockfd, "Hello, tcpclient here!\n", 23, 0);
	//close(sockfd);
	char sendbuf[4*1024] = {0};
	int i;
	for(i = 0; i < (1024/4); i++) {
		send(sockfd, sendbuf, 4*1024, 0);
	}
	close(sockfd);
	return 0;
}

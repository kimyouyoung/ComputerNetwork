#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 30
void error_handling(char *message);

int main(int argc, char **argv){
	int sock;
	char message[BUFSIZE];
	int str_len, i;

	char MSG1[] = "";

	struct sockaddr_in serv_addr;
	
	if(argc != 3){
		printf("Usage: %s <IP><port>\n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("TCP Socket create error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error\n");

	write(sock, MSG1, strlen(MSG1));


	for(i = 0; i < 4; i++){
		str_len = read(sock, message, sizeof(message) - 1);
		message[str_len] = 0;
		printf("read_return = %d\n", str_len);
		//printf("reception message from server no.%d: %s\n", i, message);
	}

	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputs("\n", stderr);
	exit(1);
}	

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
	int serv_sock;
	int clnt_sock;
	char message[BUFSIZE];
	int str_len, num = 0, i;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;

	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("TCP Socket create error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error\n");

	sleep(5);

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error\n");

	while(1){
		
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");

		while(1){
			sleep(1);
			str_len = read(clnt_sock, message, BUFSIZE);
			if(str_len == 0){
				printf("read_return = %d\n", str_len);
				break;
			}
			write(clnt_sock, message, str_len);
			printf("reception number: %d\n", num++);
		}

		str_len = read(clnt_sock, message, BUFSIZE);	
		close(clnt_sock);
	}
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputs("\n", stderr);
	exit(1);
}	

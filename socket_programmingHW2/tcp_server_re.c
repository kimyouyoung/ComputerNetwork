#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char **argv){
	
	int serv_sock;
	int clnt_sock;
	char message[BUFSIZE];
	char file_name[256];
	int str_len, i;
	char size[BUFSIZE]; 
	int check = 0;

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

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error\n");

	while(1){	
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");
		else
			printf("count\n");
	
		str_len = recv(clnt_sock, file_name, 256, 0);
		send(clnt_sock, file_name, str_len, 0);

		FILE *fp = fopen(file_name, "wb");
		if(fp == NULL)
			error_handling("Error File Open");

		while(1){
			str_len = recv(clnt_sock, size, BUFSIZE, 0);
			if((check = atoi(size)) != 0){
				printf("size: %d\n", check);
				break;
			}
			else
				printf("no size\n");
		}
	
		int len = check;
		int count = 0;
		
		while(len != 0){
			str_len = recv(clnt_sock, message, BUFSIZE, 0);
			len -= str_len;
			fwrite(message, sizeof(char), str_len, fp);
			send(clnt_sock, message, str_len, 0);
			str_len = 0;
		}
	
		
		fclose(fp);
		close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputs("\n", stderr);
	exit(1);
}	

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
	char index[BUFSIZE];
	char query[BUFSIZE];
	char post[BUFSIZE];
	char request[BUFSIZE];
	int str_len, i_size, q_size, p_size;

	char ok[BUFSIZE] = "HTTP/1.1 200 OK\r\n\n";
	char not[64] = "HTTP/1.1 404 Not Found\r\n\n";

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

	
	FILE *fp = fopen("index.html", "r");
	if(fp == NULL)
		error_handling("Error File Open!\n");

	fseek(fp, 0, SEEK_END);
	i_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(index, i_size, 1, fp);
	index[i_size] = 0;
	fclose(fp);

	FILE *q = fopen("query.html", "r");
	if(q == NULL)
		error_handling("Error File Open\n");
	fseek(q, 0, SEEK_END);
	q_size = ftell(q);
	fseek(q, 0, SEEK_SET);
	fread(query, q_size, 1, q);
	query[q_size] = 0;
	fclose(q);

	FILE *p = fopen("post.html", "r");
	if(p == NULL)
		error_handling("Error File Open\n");
	fseek(p, 0, SEEK_END);
	p_size = ftell(p);
	fseek(p, 0, SEEK_SET);
	fread(post, p_size, 1, p);
	post[p_size] = 0;
	fclose(p);


	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error\n");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error\n");

	while(1){	
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");
		
		str_len = recv(clnt_sock, request, BUFSIZE, 0);
		char *pp = strstr(request, "name");
		char *ptr = strtok(request, "H");
		
		if(!strcmp(ptr, "GET / ") || !strcmp(ptr, "GET /index.html ")){
			send(clnt_sock, ok, strlen(ok), 0);
			send(clnt_sock, index, strlen(index), 0);
			printf("INDEX\n");
		}
		else if(!strcmp(ptr, "GET /query.html ")){
			send(clnt_sock, ok, strlen(ok), 0);
			send(clnt_sock, query, strlen(query), 0);
			printf("QUERY\n");
		}
		else if(!strcmp(ptr, "POST /sample ")){
			char temp[BUFSIZE] = "";
			char *postptr = strtok(post, "Q");
			strcat(temp, postptr);
			strcat(temp, pp);
			postptr = strtok(NULL, "Q");
			strcat(temp, postptr);

			temp[strlen(temp)-1] = 0;
			send(clnt_sock, ok, strlen(ok), 0);
			send(clnt_sock, temp, strlen(temp), 0);
		}
		else{
			send(clnt_sock, not, strlen(not), 0);
			printf("NOT FOUND\n");
		}
		
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

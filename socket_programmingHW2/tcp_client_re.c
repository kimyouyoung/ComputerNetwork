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
        
	int sock;
    int str_len;
    char file_name[256];

    struct sockaddr_in serv_addr;

    if(argc != 4){
            printf("Usage: %s <IP><port><file_name>\n", argv[0]);
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

    send(sock, argv[3], strlen(argv[3]), 0);
    str_len = recv(sock, file_name, 256, 0);
    file_name[str_len] = 0;
	
	FILE *fp = fopen(argv[3], "rb");
	if(fp == NULL)
		error_handling("Error File Open");

	int byte = 0;
	fseek(fp, 0, SEEK_END);
	byte = ftell(fp);	
	char size[BUFSIZE];
	sprintf(size, "%d", byte);
	send(sock, size, strlen(size), 0);
	fseek(fp, 0l, SEEK_SET);
    
    char message[byte];
    str_len = fread(message, 1, byte, fp);
    message[str_len] = 0;
    send(sock, message, str_len, 0);

	fclose(fp);
    close(sock);
    return 0;
}

void error_handling(char *message){
        fputs(message, stderr);
        fputs("\n", stderr);
        exit(1);
}

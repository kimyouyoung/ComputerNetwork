#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char **argv){
    int sock;
    char message[BUFSIZE];
    char file_name[256];
    char oksign[4] = "ok";
    int str_len, addr_size, i;


    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    if(argc != 4){
            printf("Usage: %s <IP><port><file_name>\n", argv[0]);
            exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
            error_handling("UDP Socket create error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

	FILE *fp = fopen(argv[3], "rb");
	if(fp == NULL)
		error_handling("Error File Open");

	while(1){
        sendto(sock, argv[3], strlen(argv[3]), 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
		addr_size = sizeof(clnt_addr);
		sleep(1);
		str_len = recvfrom(sock, file_name, 256, 0, (struct sockaddr*) &clnt_addr, &addr_size);
		file_name[str_len] = 0;
		printf("file: %s\n",file_name);
		if(!strcmp(file_name, argv[3])) break;
	}
	
	sendto(sock, oksign, strlen(oksign), 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

	char rcv_oksign[4];
	while(1){
		str_len = recvfrom(sock, rcv_oksign, 4, 0, (struct sockaddr*) &clnt_addr, &addr_size);
		if(!strcmp(rcv_oksign, "ok")) break;
		else
			sleep(1);
	}

	printf("ok file name\n");

	int count = 0;
	while(feof(fp) == 0){
		str_len = fread(message, 1, BUFSIZE, fp);
		sendto(sock, message, str_len, 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
		count++;
	}
	
	printf("done\n");
	sleep(10);
	sendto(sock, "", 0, 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	fclose(fp);
        close(sock);
        return 0;
}

void error_handling(char *message){

        fputs(message, stderr);
        fputs("\n", stderr);
        exit(1);

}

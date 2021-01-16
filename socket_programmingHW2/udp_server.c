#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char **argv){
	int serv_sock;
	char message[BUFSIZE];
	char file_name[256];
	char oksign[4];
	int str_len;
	int check = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;

	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP Socket create error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error\n");
    while(1){
        while(1){
            clnt_addr_size = sizeof(clnt_addr);
            while(1){
                if((str_len = recvfrom(serv_sock, file_name, 256, 0, (struct sockaddr*) &clnt_addr, &clnt_addr_size)) < 1)
                    sleep(1);
                else{
                    while(1){
                        sendto(serv_sock, file_name, str_len, 0, (struct sockaddr*) &clnt_addr, sizeof(clnt_addr));
                        str_len = recvfrom(serv_sock, oksign, 4, 0, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
                        if(!strcmp(oksign, "ok")){
                            check = 1;
                            break;
                        }else
                            sleep(1);
                    }
                }
                if(check == 1)
                    break;
            }

            if(check == 1)
                break;
        }

        printf("%s make\n", file_name);
        FILE *fp = fopen(file_name, "wb");
        if(fp == NULL)
            error_handling("Error File Open");
        sendto(serv_sock, oksign, str_len, 0, (struct sockaddr*) &clnt_addr, sizeof(clnt_addr));
        while(1){
            clnt_addr_size = sizeof(clnt_addr);
            str_len = recvfrom(serv_sock, message, BUFSIZE, 0, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
            fwrite(message, sizeof(char), str_len, fp);
            if(str_len == 0) break;
        }
        fclose(fp);
    }
    
	close(serv_sock);
	return 0;
}

void error_handling(char *message){

	fputs(message, stderr);
	fputs("\n", stderr);
	exit(1);

}

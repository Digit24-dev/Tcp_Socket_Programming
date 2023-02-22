#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
리눅스 기반 예제를 윈도우 기반으로 변경하기 위해서 다음 네가지를 기억하자.
 1. WSAStartup, WSACleanup 함수 호출을 통한 소켓 라이브러리의 초기화와 해제
 2. 자료형과 변수의 이름을 윈도우 스타일로 변경하기
 3. 데이터 송수신을 위해서 read, write 함수 대신 recv, send 함수 호출하기
 4. 소켓의 종료를 위해서 close 대신 closesocket 함수 호출하기
*/


#ifdef  windows
    #include <winsock.h>
#endif
#ifdef  linux
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]){
    #ifdef windows
        WSADATA wsaData;
        SOCKET hSocket;
        char message[BUF_SIZE];
        int strLen;
        SOCKADDR_IN servAdr;
    #endif

    #ifdef linux
        int sock;
        char message[BUF_SIZE];
        int str_len;

        struct sockaddr_in serv_adr;
    #endif
    
    if(argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
#ifdef	linux
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected.........");
	while(1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);

		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		write(sock, message, strlen(message));
		str_len=read(sock, message, BUF_SIZE-1);
		message[str_len]=0;
		printf("Message from server: %s", message);
	}
	close(sock);
#endif

#ifdef  windows
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSocket=socket(PF_INET, SOCK_STREAM, 0);
    if(hSocket==INVALID_SOCKET)
        ErrorHandling("socket() error!");
    
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family=AF_INET;
    servAdr.sin_addr.s_addr=inet_addr(argv[1]);
    servAdr.sin_port=htons(atoi(argv[2]));

    if(connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");
    else
        puts("Connected........");

    while(1){
        fputs("Input message(Q to quit: ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if(!strcmp(message, "q\n") || strcmp(message, "Q\n"))
            break;
        
        send(hSocket, message, strlen(message), 0);
        strLen=recv(hSocket, message, BUF_SIZE-1, 0);
        message[strLen] = 0;
        printf("Message from server : %s", message);
    }
    closesocket(hSocket);
    WSACleanup();
#endif
    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

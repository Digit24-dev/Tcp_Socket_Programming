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

#define windows

#ifdef  windows
    #include <winsock.h>
#endif
#ifdef  linux
    #include <unistd.h>
    #include <arap/inet.h>
    #include <sys/socket.h>
#endif


#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char *argv){
    #ifdef windows
        WSADATA wsaData;
        SOCKET hServSock, hClntSock;
        char message[BUF_SIZE];
        int strLen, i;

        SOCKADDR_IN servAdr, clntAdr;
        int clntAdrSize;
    #endif

    #ifdef linux
        int serv_scok, clnt_sock;
        char message[BUF_SIZE];
        int str_len, i;

        struct sockaddr_in serv_adr, clnt_adr;
        socklen_t clnt_adr_sz;
    #endif
    
    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(0);
    }

#ifdef  windows
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hServSock=socket(PF_INET, SOCK_STREAM, 0);
    if(hServSock==INVALID_SOCKET)
        ErrorHandling("socket() error!");
    
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family=AF_INET;
    servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
    servAdr.sin_port=htons(atoi(argv[1]));

    if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
        ErrorHandling("bind() error");

    if(listen(hServSock, 5)==SOCKET_ERROR)
        ErrorHandling("listen() error");
    
    clntAdrSize=sizeof(clntAdr);

    for(i=0; i<5; ++i)
    {
        hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
        if(hClntSock==-1)
            ErrorHandling("accept() error!");
        else
            printf("Connect client %d \n", i+1);
        while((strLen=recv(hClntSock, message, BUF_SIZE, 0)) != 0)
            send(hClntSock, message, strLen, 0);
        closesocket(hClntSock);
    }

    closesocket(hServSock);
    WSACleanup();
#endif
    return 0;
}

void ErrorHandling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
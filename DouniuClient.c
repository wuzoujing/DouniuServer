#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>


#define MAXLINE 512
#define NAMELEN 20
#define PORT 6666
#define HOST_ADDR "127.0.0.1"

int userid = 0;
char name[32] = "";

void print_menu(void){
	printf("\t+----------------------------------+\n");
	printf("\t+            DouNiu                +\n");
	printf("\t+----------------------------------+\n");
	printf("\t+ User Commands as follows:        +\n");
	printf("\t+                                  +\n");
	printf("\t+ l: list all online user          +\n");
	printf("\t+ i: Login                         +\n");
	printf("\t+ o: logOut                        +\n");
	printf("\t+ c: Chat with other online user   +\n");
	printf("\t+ a: Chat with all online user     +\n");
	printf("\t+ h: Help                          +\n");
	printf("\t+----------------------------------+\n");
}

int get_choice(){
	printf("%s# ", name);
	int answer = getchar();	// eat <Enter>
	while(getchar() != '\n');	// eat <Enter>
	//putchar(answer);
	return answer;
}


int sockfd;
void snd();

int main()
{
    pthread_t thread;
    struct sockaddr_in servaddr;

//调用socket函数创建客户端的套接字
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        printf("Socket create failed\n");
        return -1;
    }

//初始化服务器端的地址
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    if(inet_aton(HOST_ADDR,&servaddr.sin_addr)<0)
    {
        printf("inet_aton error.\n");
        return -1;
    }

//调用connect函数实现与服务器端建立连接
    printf("Connecting...\n");
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        printf("Connect server failed.\n");
        return  -1;
    }

//从此处开始程序分做两个线程

//创建发送消息的线程，调用了发送消息的函数snd
    pthread_create(&thread,NULL,(void*)(&snd),NULL);

//从此处开始向下为接收消息的线程
    char buff[MAXLINE];
    int len;

    while(1)
    {
        if((len=read(sockfd,buff,MAXLINE))>0)
        {
            buff[len]=0;
            printf("\n%s\n\n",buff);
        }
    }

    return 0;
}

//发送消息的函数snd
void snd()
{
    char name[NAMELEN];
    char buff[MAXLINE];
    printf("thread snd[id:%lu] created\n", pthread_self());
    gets(name);
    write(sockfd,name,strlen(name));
    while(1)
    {
        gets(buff);
        write(sockfd,buff,strlen(buff));
        if(strcmp("bye",buff)==0)
        {
            exit(0);
        }
    }
}

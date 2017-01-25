#include "DouniuServer.h"

#ifdef USE_IN_ANDROID
#include <android/log.h>

#define TAG "[wzj][jni]DouniuServer"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , TAG, __VA_ARGS__)
#define printf LOGV
#endif	//USE_IN_ANDROID


int listenfd,connfd[MAX_USERS];//分别记录服务器端的套接字与连接的多个客户端的套接字
UserInfo s_users[MAX_USERS];
Card s_cards[COUNT_CARDS];

int initAndAccept()
{
	pthread_t thread;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    time_t ticks;
    char buff[MAXLINE];

//调用socket函数创建服务器端的套接字
    printf("Socket...\n");
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd<0)
    {
        printf("Socket created failed:%s\n",strerror(errno));
        return ERR;
    }

//调用bind函数使得服务器端的套接字与地址实现绑定
    printf("Bind...\n");
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        printf("Bind failed:%s\n",strerror(errno));
        return ERR;
    }

//调用listen函数，将一个主动连接套接字变为被动的倾听套接字
//在此过程中完成tcp的三次握手连接
    printf("listening...\n");
    listen(listenfd,LISTENQ);

//创建一个线程，对服务器程序进行管理（关闭）
    pthread_create(&thread,NULL,(void*)(&quit),NULL);

//记录空闲的客户端的套接字描述符（-1为空闲）
    int i=0;
    for(i=0;i<MAX_USERS;i++)
    {
        connfd[i]=-1;
		s_users[i].id = -1;
		strcpy(s_users[i].name, "");
		strcpy(s_users[i].ipaddr, "");
		s_users[i].isPrepared = FALSE;
    }

    while(1)
    {
        len=sizeof(cliaddr);
        for(i=0;i<MAX_USERS;i++)
        {
            if(connfd[i]==-1)
            {
                break;
            }
        }

//调用accept从listen接受的连接队列中取得一个连接
        connfd[i]=accept(listenfd,(struct sockaddr*)&cliaddr,&len);

        ticks=time(NULL);
        sprintf(buff,"%.24s\r\n",ctime(&ticks));
		strcpy(s_users[i].ipaddr, inet_ntoa(cliaddr.sin_addr));
        printf("%s Connect from: %s,port %d\n\n",buff,inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

//针对当前套接字创建一个线程，对当前套接字的消息进行处理
        pthread_create(malloc(sizeof(pthread_t)),NULL,(void*)(&receiveCMD),(void*)i);
    }
	return OK;
}

void disconnect()
{
	printf("[disconnect]Byebye...\n");
	close(listenfd);
#ifndef USE_IN_ANDROID
    exit(0);
#endif	//USE_IN_ANDROID
}

int main()
{
	if (initAndAccept() != OK)
	{
		printf("Occur some error, need exit.\n");
        return  -1;
	}
	printf("exit.\n");
    return 0;
}

//服务器关闭函数,若输入quit则退出服务器
void quit()
{
    char msg[10];
    printf("[quit]thread quit[id:%lu] created\n", pthread_self());
    while(1)
    {
        scanf("%s",msg);
        if(strcmp("quit",msg)==0)
        {
			disconnect();
        }
    }
}

//服务器接收并转发消息函数
//  每个client都对应一个接收线程，n为clientID
//  接收client的请求消息后，调用processMsg来处理消息，然后反馈消息给client
void receiveCMD(int n)
{
    char buff1[MAXLINE];
	int len;
    printf("[rcv_snd]thread rcv_snd[n:%d][id:%lu] created\n", n, pthread_self());

//接受当前用户的信息并将其转发给所有的用户
    while(1)
    {
        if((len=read(connfd[n],buff1,MAXLINE))>0)
        {
            buff1[len]=0;
			printf("\n[S<-C][rcv_snd]buff1: %s, n:%d\n", buff1, n);
			processMsg(buff1, n);
        }
    }
}

//消息处理关键函数
//  buffer为请求消息字符串，n为clientID
int processMsg(char* buffer, int n)
{
	char * data[DATA_LEN];
	char * str, *subtoken;
	char sendBuff[MAXLINE];
	int i;
	memset(data, 0, sizeof(data));

	//按协议格式解析消息，然后存放到data数组
	for(str = buffer, i = 0; ; str = NULL, i++){
		subtoken = strtok(str, DATA_TOK);
		if(subtoken == NULL)
			break;
		data[i] = subtoken;
		printf("[processMsg]> data[%d] = %s\n", i, subtoken);
	}

	char info[256];
	char buf[256];
	bool hasSomeoneNotPrepared = TRUE;
	int countUsers = 0;
	
	// 处理消息
	switch(data[OFT_CMD][0])
	{
	case CMD_LIST:
		{
			printf("[processMsg]CMD_LIST\n");
			memset(buf, 0, sizeof(buf));
			for(i=0;i<MAX_USERS;i++)
			{
				if (s_users[i].id != -1)
				{
					sprintf(info, "%d#%s#%ld#", s_users[i].id, s_users[i].name, s_users[i].login_time);
					strcat(buf, info);
				}
			}
			if (strlen(buf) > 0)
			{
				buf[strlen(buf) - 1] = 0;
			}

			sprintf(sendBuff, "%c:%d:%ld:%s", CMD_LIST, n, time(NULL), buf);
			printf("[S->C][processMsg]sendBuff:%s\n",sendBuff);

			write(connfd[n],sendBuff,strlen(sendBuff));
		}
		break;
	case CMD_LOGIN:
		{
			printf("[processMsg]CMD_LOGIN\n");
			s_users[n].id = n;
			s_users[n].login_time = atol(data[OFT_TIM]);
			strcpy(s_users[n].name, data[OFT_FRM]);

			sprintf(sendBuff, "%c:%d:%ld:%s\t%s\0", CMD_LOGIN, n, time(NULL), data[OFT_FRM], "join in");
			printf("[S->C][processMsg]sendBuff:%s\n",sendBuff);

			//反馈login OK消息给client
			write(connfd[n],sendBuff,strlen(sendBuff));
			
			//把新用户的加入告知其他用户
			for(i=0;i<MAX_USERS;i++)
			{
				if(i!=n && connfd[i]!=-1)
				{
					printf("[S->C][processMsg]i:%d notify other users\n",i);
					write(connfd[i],sendBuff,strlen(sendBuff));
				}
				else if (i==n)
				{
					printf("[processMsg]i:%d itself, ip:%s\n",i, s_users[n].ipaddr);
				}
				else
				{
					printf("[processMsg]i:%d connfd null\n",i);
				}
			}
		}
		break;
	case CMD_LOGOUT:
		{
			printf("[processMsg]CMD_LOGOUT\n");
			sprintf(sendBuff, "%c:%d:%ld:%s", CMD_LOGOUT, n, time(NULL), "logout ok!");
			printf("[S->C][processMsg]sendBuff:%s\n",sendBuff);
			write(connfd[n],sendBuff,strlen(sendBuff));
			s_users[n].id = -1;
			strcpy(s_users[n].name, "");
			strcpy(s_users[n].ipaddr, "");
			s_users[n].isPrepared = TRUE;
			printf("[processMsg]exit thread\n");
			close(connfd[n]);
			connfd[n] = -1;
			pthread_exit(0);
		}
		break;
	case CMD_PREPARE:
		{
			printf("[processMsg]CMD_PREPARE\n");
			s_users[n].isPrepared = TRUE;
			sprintf(sendBuff, "%c:%d:%ld:%s", CMD_PREPARE, n, time(NULL), "prepared");
			printf("[S->C][processMsg]sendBuff:%s\n",sendBuff);
			
			hasSomeoneNotPrepared = FALSE;
			memset(sendBuff, 0, sizeof(sendBuff));
			for(i=0;i<MAX_USERS;i++)
			{
				if (s_users[i].id != -1)
				{
					countUsers++;
					if (!s_users[i].isPrepared)
					{
						hasSomeoneNotPrepared = TRUE;
						break;
					}
				}
			}
			
			//用户数超过2且所有用户都准备就绪，将发牌
			printf("[processMsg]countUsers:%d,will fapai\n",countUsers);
			if (countUsers >= 1 && !hasSomeoneNotPrepared)
			{
				printf("[processMsg]->initializePai\n");
				initializePai(s_cards, COUNT_CARDS);
				printf("[processMsg]->xiPai\n");
				xiPai(s_cards,COUNT_CARDS);
				printf("[processMsg]->faPai\n");
				faPai(s_users, MAX_USERS, s_cards, COUNT_CARDS);
				
				//TODO:send to client
				memset(buf, 0, sizeof(buf));
				for(i=0;i<MAX_USERS;i++)
				{
					if (s_users[i].id != -1)
					{
						sprintf(info, "%d#%s#%d#%d#%d#%d#%d#", s_users[i].id, s_users[i].name, s_users[i].gameInfo.cards[0].id, 
							s_users[i].gameInfo.cards[1].id, s_users[i].gameInfo.cards[2].id, s_users[i].gameInfo.cards[3].id, 
							s_users[i].gameInfo.cards[4].id);
						strcat(buf, info);
					}
				}
				if (strlen(buf) > 0)
				{
					buf[strlen(buf) - 1] = 0;
				}
				sprintf(sendBuff, "%c:%d:%ld:%s", CMD_START, n, time(NULL), buf);
				printf("[S->C][processMsg]sendBuff:%s\n",sendBuff);
				
				for (i=0;i<MAX_USERS;i++)
				{
					if (s_users[i].id != -1)
					{
						write(connfd[i],sendBuff,strlen(sendBuff));
					}
				}
			}
			else
			{
				printf("[processMsg]wait more users to prepare\n");
			}
		}
		break;
	case CMD_TRYINGBANKER:
		{
			printf("[processMsg]CMD_TRYINGBANKER\n");
			//TODO: 庄家如何决定?时间先后还是随机?
			
		}
		break;
	case CMD_STAKE:
		{
			printf("[processMsg]CMD_STAKE\n");
			
		}
		break;
	case CMD_PLAY:
		{
			printf("[processMsg]CMD_PLAY\n");
			
		}
		break;
	default:
		return ERR;
	}

    return OK;
}


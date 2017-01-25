#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include <fcntl.h>
//#include <sys/shm.h>
#include <pthread.h>
#include "msg.h"
#include "DouniuStruct.h"

#define MAXLINE 512
#define PORT 6666
#define MAX_IP_ADDR_LEN 20

#ifdef USE_IN_ANDROID
#define DEFAULT_HOST_ADDR "192.168.1.139"
#else 
#define DEFAULT_HOST_ADDR "127.0.0.1"
#endif

#ifndef _Included_DouniuClient_H
#define _Included_DouniuClient_H

typedef struct CLIENT_USER_INFO {
	int id;
	char name[MAX_NAME_LEN];
	bool isPrepared;
	enum TRYING_BANKER_STATUS bankerStatus;
	int stake;
	GameInfo gameInfo;
}ClientUserInfo;

extern int userid;
extern char name[MAX_NAME_LEN];
extern int sockfd;
extern pthread_t thread;
extern ClientUserInfo myInfo;
extern int bankerIndex;

//extern void print_menu(void);
//extern int get_choice();
//extern int loginTest();
extern int loginCMD(char *username, char *password);
extern void afterLoginCMD();
extern int logoutCMD(char *username);
extern void listAllUsers();
extern void format_user_list(char * buffer);
extern void prepareCMD();
extern void format_game_info(char * buffer);
extern void tryingBankerCMD(int value);
extern void stakeCMD(int stakeValue);
extern void submitCMD(int niuValue);
extern int connectServer(char *ipaddr);
extern char* connectServerAndLogin(char *ipaddr, char *username, char *password);
extern int logoutAndExit(char *username);
extern void * receiver_looper(void * p);

#endif	//_Included_DouniuClient_H

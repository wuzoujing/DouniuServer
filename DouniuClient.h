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
#define DEFAULT_HOST_ADDR "127.0.0.1"
//#define DEFAULT_HOST_ADDR "192.168.1.122"

#ifndef _Included_DouniuClient_H
#define _Included_DouniuClient_H


extern int userid;
extern char name[MAX_NAME_LEN];
extern int sockfd;
extern pthread_t thread;

//extern void print_menu(void);
//extern int get_choice();
extern int login();
extern int loginCMD(char *username, char *password);
extern void afterLoginCMD();
extern int logoutCMD(char *username);
extern void listAllUsers();
extern void format_user_list(char * buffer);
extern void prepareCMD();
extern void format_game_info(char * buffer);
extern void tryingBankerCMD();
extern void stakeCMD();
extern void playCMD();
extern int connectServer(char *ipaddr);
extern int connectServerAndLogin(char *ipaddr, char *username, char *password);
extern void * receiver_looper(void * p);

#endif	//_Included_DouniuClient_H

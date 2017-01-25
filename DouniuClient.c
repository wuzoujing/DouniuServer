#include "DouniuClient.h"

#ifdef USE_IN_ANDROID
#include <android/log.h>

#define TAG "[wzj][jni]DouniuClient"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , TAG, __VA_ARGS__)
#define printf LOGV
#endif	//USE_IN_ANDROID


int userid = 0;
char hostAddr[MAX_IP_ADDR_LEN] = DEFAULT_HOST_ADDR;
char name[MAX_NAME_LEN] = "";
int sockfd;
pthread_t thread;

void print_menu(void){
	printf("\t+----------------------------------+\n");
	printf("\t+            DouNiu                +\n");
	printf("\t+----------------------------------+\n");
	printf("\t+ User Commands as follows:        +\n");
	printf("\t+                                  +\n");
	printf("\t+ l: List all online user          +\n");
	printf("\t+ i: logIn                         +\n");
	printf("\t+ o: logOut                        +\n");
	printf("\t+ c: Chat with other online user   +\n");
	printf("\t+ a: chat with All online user     +\n");
    printf("\t+ p: Prepare                       +\n");
    printf("\t+ b: trying Banker                 +\n");
    printf("\t+ s: Stake(etc. 1x,5x,10x)         +\n");
    printf("\t+ y: plaY                          +\n");
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

//send:CMD:FROM:TIME:DATA
//recv:CMD:userid:TIME:DATA, userid即第几个
int login(){
	char inputname[MAX_NAME_LEN] = "";
	int ret = OK;
	printf("username: \n");
	if(fgets(inputname, sizeof(inputname), stdin) == NULL){
		perror("fgets");
		return ERR;
	}
	inputname[strlen(inputname) - 1] = 0;
	ret= loginCMD(inputname, "");
	if (ret == OK)
	{
		printf("login success, will create thread to receive response from server\n");
		afterLoginCMD();
		return OK;
	}
	return ret;
}

int loginCMD(char *username, char *password)
{
	char buff[MAXLINE];
	int rand_type = random();
	printf("[loginCMD]username:%s,password:%s\n", username, password);
	if (!username || !password)
	{
		printf("username or password null, please input\n");
		return ERR;
	}
	if (strlen(username) > MAX_NAME_LEN)
	{
		printf("username or password null, please input\n");
		return ERR;
	}

	strcpy(name, username);
	
	time_t t;
	time(&t);
	sprintf(buff, "%c:%s:%ld:%d", CMD_LOGIN, username, t, rand_type);
	printf("[C->S][loginCMD]buff: %s\n", buff);

	//get a random type to login server
	write(sockfd,buff,strlen(buff));

	//wait server response
	printf("[loginCMD]wait rep:\n");
	int len = 0;
	char * data[DATA_LEN];
	char * str, *subtoken;
	int i;
	if((len=read(sockfd,buff,MAXLINE))>0)
    {
        buff[len]=0;
        printf("[C<-S][loginCMD]resp: %s\n\n",buff);

		memset(data, 0, sizeof(data));
		for(str = buff, i = 0; ; str = NULL, i++){
			subtoken = strtok(str, DATA_TOK);
			if(subtoken == NULL)
				break;
			data[i] = subtoken;
			printf("> data[%d] = %s\n", i, subtoken);
		}
		userid = atoi(data[OFT_FRM]);
		printf("userid:%d\n", userid);
		return OK;
    }
	return ERR;
}

void afterLoginCMD()
{
	while(pthread_create(&thread, NULL, receiver_looper, NULL) < 0);
}

//CMD:FROM:TIME:DATA
int logoutCMD(char *username){
	if(strlen(username) == 0){
		return ERR;
	}

    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%s", CMD_LOGOUT, userid, time(NULL), "Logout");
	printf("[C->S][logout]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));

	//wait server response in receiver_looper
	printf("[logout]wait rep:\n");
	return OK;
}

void listAllUsers() {
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%s", CMD_LIST, userid, time(NULL), "List");
	printf("[C->S][logout]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
}

void format_user_list(char * buffer){
	char * data[USER_DATA_LEN];
	char * str, *subtoken;
	char time_buf[24];
	int i, n;
	memset(data, 0, sizeof(data));
	printf("----------------------------------\n");	
	printf(" _ID_  USR_NAME  Login_Time \n");	
	printf("----------------------------------\n");	
	for(str = buffer, i = 0, n = 1; ; str = NULL, i++){
		subtoken = strtok(str, USER_DATA_TOK);
		if(subtoken == NULL)
			break;
		data[i] = subtoken;
		if(i != 0 && i % 2 == 0){
			printf(" %4s  %8s  %s\n", data[OFT_USR_ID], data[OFT_USR_NM], time2str(atol(data[OFT_LOGIN_TM]), time_buf));	
			n++;
			i = -1;
		}
	}
}

void prepareCMD()
{
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%s", CMD_PREPARE, userid, time(NULL), "Prepare");
	printf("[C->S][startPrepare]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
}

void format_game_info(char * buffer){
	char * data[GAME_DATA_LEN];
	char * str, *subtoken;
	int i, n;
	memset(data, 0, sizeof(data));
	printf("[format_game_info]buffer: %s\n", buffer);
	for(str = buffer, i = 0, n = 1; ; str = NULL, i++){
		subtoken = strtok(str, GAME_DATA_TOK);
		if(subtoken == NULL)
			break;
		data[i] = subtoken;
		printf("[format_game_info]str:%s, subtoken: %s, i:%d\n", str, subtoken,i);
		if(i != 0 && i % 6 == 0){
			printf(" %4s  %8s  %s  %s  %s  %s  %s\n", data[OFT_USR_ID], data[OFT_USR_NM], data[OFT_CARD1], data[OFT_CARD2], data[OFT_CARD3], data[OFT_CARD4], data[OFT_CARD5]);	
			n++;
			i = -1;
		}
	}
}

void tryingBankerCMD()
{
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%s", CMD_TRYINGBANKER, userid, time(NULL), "1");//"1":trying, "0":skip
	printf("[C->S][startTryingBanker]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
}

void stakeCMD()
{
    char buff[MAXLINE];
	int stake = 5;
	sprintf(buff, "%c:%d:%ld:%d", CMD_STAKE, userid, time(NULL), stake);
	printf("[C->S][startPrepare]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
}

void playCMD()
{
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%s", CMD_PLAY, userid, time(NULL), "pattern");//pattern, points and maxCardValue
	printf("[C->S][startPrepare]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
}

int connectServer(char *ipaddr)
{
	struct sockaddr_in servaddr;

	//调用socket函数创建客户端的套接字
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        printf("Socket create failed:%s\n",strerror(errno));
        return ERR;
    }

	//初始化服务器端的地址
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
	if (ipaddr != NULL)
	{
		strcpy(hostAddr, ipaddr);
	}
	printf("hostAddr[%s],ipaddr[%s]\n",hostAddr,ipaddr);
    if(inet_aton(hostAddr,&servaddr.sin_addr)<0)
    {
    	printf("inet_aton error:%s\n",strerror(errno));
        return ERR;
    }
	//mysock.sin_addr.s_addr = inet_addr(hostAddr);

	//调用connect函数实现与服务器端建立连接
    printf("Connecting [%s] ...\n", hostAddr);
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
    	printf("Connect server failed:%s\n",strerror(errno));
        return  ERR;
    }
	return OK;
}

int connectServerAndLogin(char *ipaddr, char *username, char *password)
{
	printf("ipaddr[%s],username[%s],password[%s]\n", ipaddr, username, password);
	int ret = OK;
	ret = connectServer(ipaddr);
	if (ret != OK)
	{
		printf("Connect [%s] fail.\n", ipaddr);
		return ERR;
	}

	//账户登录并创建线程接收来自服务器的反馈
	printf("login as user: [%s] ...\n", username);
	return loginCMD(username, password);
}

//login时创建的接收线程，处理服务器的消息
void * receiver_looper(void * p){
    char buff[MAXLINE];
    int len;
	
	char * data[DATA_LEN];
	char * str, *subtoken;
	int i;
	
    while(1)
    {
        if((len=read(sockfd,buff,MAXLINE))>0)
        {
            buff[len]=0;
            printf("[C<-S][receiver_looper]buff:%s\n",buff);

			memset(data, 0, sizeof(data));
			for(str = buff, i = 0; ; str = NULL, i++){
				subtoken = strtok(str, DATA_TOK);
				if(subtoken == NULL)
					break;
				data[i] = subtoken;
				printf("> data[%d] = %s\n", i, subtoken);
			}
			//printf("==%d==\n",__LINE__);
			
			// process received data
			// data format error
			//if(i != DATA_LEN)
			//	continue;

			switch(data[OFT_CMD][0]){
			case CMD_LIST:
				//if(strcmp(data[OFT_FRM], TYPE_SERVER_STR)){
				//	continue;
				//}
				//format_user_list(data[OFT_DAT]);
				printf("[receiver_looper]CMD_LIST\n");
				format_user_list(data[OFT_DAT]);
				break;
			case CMD_LOGOUT:
				printf("[receiver_looper]CMD_LOGOUT\n");
				//if(strcmp(data[OFT_FRM], TYPE_SERVER_STR)){
				//	continue;
				//}
				printf("> %s \n", data[OFT_DAT]);
				//printf("%s\n", time2str(atol(data[OFT_TIM]), data[OFT_DAT]));
				exit(0);
			case CMD_CHAT:		// print chat content
			    printf("[receiver_looper]CMD_CHAT\n");
				break;
			case CMD_PREPARE:
				printf("[receiver_looper]CMD_PREPARE\n");
				printf("[receiver_looper]%s\n",data[OFT_DAT]);
				break;
			case CMD_START:
				printf("[receiver_looper]CMD_START\n");
				format_game_info(data[OFT_DAT]);
				//TODO
				break;
			default:
				break;
			}
			//printf("%s# ", name);
        }
    }
}

int main()
{
    if (connectServerAndLogin(DEFAULT_HOST_ADDR, "test", "123") != OK)
    //if (connectServer(DEFAULT_HOST_ADDR) != OK)
    {
		printf("Occur some error, need exit.\n");
        return  -1;
    }

//主线程处理发送（可命令行选择输入），一旦login则创建子线程处理接收。
    print_menu();
    while(1){
        //printf("main loop\n");
        switch(get_choice()){
        case 'i':
            login();
            //while(pthread_create(&thread, NULL, receiver_looper, NULL) < 0);
            break;
        case 'l':
            {
                printf("before CMD_LIST\n");
                listAllUsers();
                printf("after CMD_LIST\n");
                sleep(1);
            }
            break;
        case 'o':
            if(logoutCMD(name) == OK){
				printf("login quit by pthread_join\n");
                pthread_join(thread, NULL);
                return 0;
            }else{
                printf("Not login quit\n");
                return 0;
            }
            break;
        case 'c':
            //chat();
            break;
        case 'a':
            //chat_all();
            break;
	    case 'p':
            prepareCMD();
            break;
		case 'b':
            tryingBankerCMD();
            break;
		case 's':
            stakeCMD();
            break;
		case 'y':
            playCMD();
            break;
        case 'h':
            print_menu();
            break;
        }
    }
	return 0;
}

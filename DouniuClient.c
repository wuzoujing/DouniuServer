#include "DouniuClient.h"
#include "GameRule.h"

#ifdef USE_IN_ANDROID
#include <android/log.h>

#define TAG "[wzj][c]DouniuClient"
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
ClientUserInfo myInfo;
ClientUserInfo otherUsers;
int bankerIndex = 0;
char retLoginCMD[MAXLINE] = "";

char *time2str(long time, char* buf){
	struct tm *t = localtime(&time);
	strftime(buf, 32, "%Y-%m-%d-%H:%M:%S", t);
	return buf;
}

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
int loginTest(){
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
		if (buff[len-1]=='\n')
		{
			buff[len-1]=0;
		}
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
		strcpy(name, username);
		myInfo.id = userid;
		strcpy(myInfo.name,username);
		printf("userid:%d\n", userid);
#ifdef USE_IN_ANDROID
		//loginCb(userid, data[OFT_DAT], strlen(data[OFT_DAT]));
		sprintf(retLoginCMD, "%d:%s", userid, data[OFT_DAT]);
#endif
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
	printf("[C->S][listAllUsers]send buff: %s\n", buff);
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
	myInfo.isPrepared = TRUE;
}

void format_game_info(char * buffer){
	char * data[GAME_DATA_LEN];
	char * str, *subtoken;
	int i, n;
	GameInfo gameInfo;
	Card tempCard;
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
			if (userid == atoi(data[OFT_USR_ID]))
			{
				gameInfo.cards[0] = updateCardById(atoi(data[OFT_CARD1]));
				gameInfo.cards[1] = updateCardById(atoi(data[OFT_CARD2]));
				gameInfo.cards[2] = updateCardById(atoi(data[OFT_CARD3]));
				gameInfo.cards[3] = updateCardById(atoi(data[OFT_CARD4]));
				gameInfo.cards[4] = updateCardById(atoi(data[OFT_CARD5]));
			}
			n++;
			i = -1;
		}
	}
}

void tryingBankerCMD(int value)
{
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%d", CMD_TRYINGBANKER, userid, time(NULL), value);// 1:trying, 0:skip
	printf("[C->S][startTryingBanker]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
	myInfo.bankerStatus = TBS_TRYING;
}

void stakeCMD(int stakeValue)
{
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%d", CMD_STAKE, userid, time(NULL), stakeValue);
	printf("[C->S][startPrepare]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
	myInfo.stake = stakeValue;
}

void playCMD(int niuValue)
{
    char buff[MAXLINE];
	sprintf(buff, "%c:%d:%ld:%d", CMD_PLAY, userid, time(NULL), niuValue);
	printf("[C->S][startPrepare]send buff: %s\n", buff);
	write(sockfd,buff,strlen(buff));
}

void playTest()
{
	//char niustr[MAX_NAME_LEN] = "";
	int value = 0;
	printf("is there niu? (1 or 0) \n");
	//if(fgets(niustr, sizeof(niustr), stdin) == NULL){
	//	perror("fgets");
	//	return ERR;
	//}

	int answer = getchar();	// eat <Enter>
	while(getchar() != '\n');	// eat <Enter>
	if (answer == '1')
	{
		value = 1;
	}
	else
	{
		value = 0;
	}
	playCMD(value);
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

char* connectServerAndLogin(char *ipaddr, char *username, char *password)
{
	printf("ipaddr[%s],username[%s],password[%s]\n", ipaddr, username, password);
	int ret = connectServer(ipaddr);
	if (ret != OK)
	{
		printf("Connect [%s] fail.\n", ipaddr);
		return "";
	}

	//账户登录并创建线程接收来自服务器的反馈
	printf("login as user: [%s] ...\n", username);
	ret = loginCMD(username, password);
	if (ret == OK)
	{
		printf("login success, will create thread to receive response from server\n");
		afterLoginCMD();
		return retLoginCMD;
	}
	return "";
}

int logoutAndExit(char *username)
{
	int ret = logoutCMD(username);
	if (ret == OK)
	{
		printf("login quit by pthread_join\n");
        pthread_join(thread, NULL);
	}
	else
	{
        printf("Not login quit\n");
    }
	return ret;
}

//login时创建的接收线程，处理服务器的消息
void * receiver_looper(void * p){
    char buff[MAXLINE];
    int len;
	
	char * data[DATA_LEN];
	char * str, *subtoken;
	int i;

	char currBuff[MAXLINE];
	
    while(1)
    {
        if((len=read(sockfd,buff,MAXLINE))>0)
        {
            buff[len]=0;
			if (buff[len-1]=='\n')
			{
				buff[len-1]=0;
			}
			printf("[C<-S][receiver_looper]buff:%s\n",buff);

			char * pLeft = buff;
			char * pos = NULL;
			do
			{
				memset(currBuff, 0, sizeof(currBuff));
				pos = strchr(pLeft, '\n');
				if (pos)
				{
					//printf(">>>>>>pos not NULL\n");
					int currSize = pos - pLeft;
					strncpy(currBuff,pLeft,currSize);
					pLeft = pos + 1;
				}
				else
				{
					//printf(">>>>>>pos NULL\n");
					strcpy(currBuff,pLeft);
				}
				if (strlen(currBuff) <= 1)
				{
					//printf(">>>>>>currBuff only one char, skip\n");
					continue;
				}

				memset(data, 0, sizeof(data));
				for(str = currBuff, i = 0; ; str = NULL, i++){
					subtoken = strtok(str, DATA_TOK);
					if(subtoken == NULL)
						break;
					data[i] = subtoken;
					printf("> data[%d] = %s\n", i, subtoken);
				}

				switch(data[OFT_CMD][0]){
				case CMD_LIST:
					printf("[receiver_looper]CMD_LIST\n");
					format_user_list(data[OFT_DAT]);
					break;
				case CMD_S2C_USER_IN:
					printf("[receiver_looper]CMD_S2C_USER_IN\n");
#ifdef USE_IN_ANDROID
					otherLoginCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
					
				case CMD_S2C_USER_OUT:
					printf("[receiver_looper]CMD_S2C_USER_OUT\n");
					break;
				case CMD_LOGOUT:
					printf("[receiver_looper]CMD_LOGOUT\n");
					printf("> %s \n", data[OFT_DAT]);
					exit(0);

				case CMD_PREPARE:
					printf("[receiver_looper]CMD_PREPARE\n");
#ifdef USE_IN_ANDROID
					prepareCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_USER_PREP:
					printf("[receiver_looper]CMD_S2C_USER_PREP\n");
#ifdef USE_IN_ANDROID
					otherUserPrepareCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_WILL_BANKER:
					printf("[receiver_looper]CMD_S2C_WILL_BANKER\n");
#ifdef USE_IN_ANDROID
					willBankerCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
					
				case CMD_TRYINGBANKER:
					printf("[receiver_looper]CMD_TRYINGBANKER\n");
#ifdef USE_IN_ANDROID
					tryBankerCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_WILL_STAKE:
					printf("[receiver_looper]CMD_S2C_WILL_STAKE\n");
#ifdef USE_IN_ANDROID
					willStakeCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
					
				case CMD_STAKE:
					printf("[receiver_looper]CMD_STAKE\n");
#ifdef USE_IN_ANDROID
					stakeCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_STAKE_VALUE:
					printf("[receiver_looper]CMD_S2C_STAKE_VALUE\n");
#ifdef USE_IN_ANDROID
					otherUserStakeValueCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_WILL_START:
					printf("[receiver_looper]CMD_S2C_WILL_START\n");
#ifdef USE_IN_ANDROID
					willStartCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#else
					format_game_info(data[OFT_DAT]);
#endif
					break;

				case CMD_PLAY:
					printf("[receiver_looper]CMD_PLAY\n");
#ifdef USE_IN_ANDROID
					playCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_CARD_PATTERN:
					printf("[receiver_looper]CMD_S2C_CARD_PATTERN\n");
#ifdef USE_IN_ANDROID
					otherUserCardPatternCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
				case CMD_S2C_GAME_RESULT:
					printf("[receiver_looper]CMD_S2C_GAME_RESULT\n");
#ifdef USE_IN_ANDROID
					gameResultCb(data[OFT_DAT], strlen(data[OFT_DAT]));
#endif
					break;
					
				case CMD_CHAT:		// print chat content
				    printf("[receiver_looper]CMD_CHAT\n");
					break;
				default:
					break;
				}
			} while (pos != NULL && pLeft != NULL);
        }
    }
}

int main()
{
    //if (connectServerAndLogin(DEFAULT_HOST_ADDR, "test", "123") == "")
    if (connectServer(DEFAULT_HOST_ADDR) != OK)
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
            loginTest();
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
			logoutAndExit(name);
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
            tryingBankerCMD(1);
            break;
		case 's':
            stakeCMD(5);
            break;
		case 'y':
            playTest();//playCMD(1);
            break;
        case 'h':
            print_menu();
            break;
        }
    }
	return 0;
}

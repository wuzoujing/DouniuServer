#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MAXLINE 512
#define		DATA_LEN	4

int main (void)
{
	printf("main start\n");
	
    char buff[MAXLINE];

	char currBuff[MAXLINE];


	
			char * pLeft = buff;
			char * pos = NULL;
			
			strcpy(buff, "P:0:1475218098:prepare ok\nR:0:1473229652:1\nE:0:1473248562:WILL_BANKER\n");
	
			do
			{
				printf("pLeft:%s\n\n",pLeft);
				memset(currBuff, 0, sizeof(currBuff));
				pos = strchr(pLeft, '\n');
				printf("pos:%s\n\n",pos);
				if (pos)
				{
					printf(">>>>>>pos not NULL\n");
					int currSize = pos - pLeft;
					strncpy(currBuff,pLeft,currSize);
					printf("if>>>>>>currSize:%d,currBuff:%s\n\n",currSize,currBuff);
					pLeft = pos + 1;
				}
				else
				{
					printf(">>>>>>pos NULL\n");
					strcpy(currBuff,pLeft);
					printf("el>>>>>>currBuff:%s\n\n",currBuff);
				}
			}while(pos != NULL);


	
	printf("main end\n");
	return 0;
}
#include "GameLogic.h"

#define DOUNIU//TEST
#define MAX_USERS 3

Card s_allCards[COUNT_CARDS];
GameInfo s_players[MAX_USERS];

void test(int str[], int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
		str[i]*=2;
        printf("%d\n", str[i]);
    }
}

int main (void)
{
	printf("main start\n");

#ifdef DOUNIU
	initializePai(s_allCards,COUNT_CARDS);
	xiPai(s_allCards,COUNT_CARDS);
	faPai(s_players, MAX_USERS, s_allCards, COUNT_CARDS);

	printf("test\n");
	Card tempCards[5];
	tempCards[0]=updateCardById(5);
	tempCards[1]=updateCardById(18);
	tempCards[2]=updateCardById(31);
	tempCards[3]=updateCardById(44);
	tempCards[4]=updateCardById(45);
	//bool ret = CheckZhaDan(tempCards, 5);
	int ret = GetMaxCardValue(tempCards, 5);
	printf("test ret:%d\n",ret);
#endif

#ifdef TEST
	int str[3];
	str[0]=1;
	str[1]=3;
	str[2]=5;
	test(str, 3);
	printf("after\n");
	int i=0;
	for(i=0;i<3;i++)
	{
		printf("%d\n",str[i]);
	}
#endif
	
	printf("main end\n");
	return 0;
}
OBJS := DouniuServer DouniuClient
all: $(OBJS)

DouniuServer: DouniuServer.c DouniuServer.h GameLogic.c GameLogic.h GameRule.c GameRule.h DouniuStruct.h msg.h
	gcc -o $@ $^ -D_DEBUG -lpthread

DouniuClient: DouniuClient.c DouniuClient.h GameRule.c GameRule.h DouniuStruct.h msg.h 
	gcc -o $@ $^ -D_DEBUG -lpthread

GameLogicTest: GameLogicTest.c GameLogic.c GameLogic.h GameRule.c GameRule.h
	gcc -o $@ $^

clean:
	$(RM) $(OBJS)

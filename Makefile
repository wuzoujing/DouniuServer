OBJS := DouniuServer DouniuClient
all: $(OBJS)

DouniuServer: DouniuServer.c DouniuServer.h DouniuStruct.h msg.h GameLogic.c GameLogic.h
	gcc -o $@ $^ -D_DEBUG -lpthread

DouniuClient: DouniuClient.c DouniuClient.h DouniuStruct.h msg.h
	gcc -o $@ $^ -D_DEBUG -lpthread

GameLogicTest: GameLogicTest.c GameLogic.c GameLogic.h
	gcc -o $@ $^

clean:
	$(RM) $(OBJS)

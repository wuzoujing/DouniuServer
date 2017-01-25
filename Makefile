OBJS := DouniuServer DouniuClient sqliteTest
all: $(OBJS)

DouniuServer: DouniuServer.c DouniuServer.h GameLogic.c GameLogic.h GameRule.c GameRule.h DouniuStruct.h msg.h sqliteHelper.c sqliteHelper.h
	gcc -o $@ $^ -D_DEBUG -lpthread -lsqlite3 -I/home7/justinwu/lib/include -L/home7/justinwu/lib/lib

DouniuClient: DouniuClient.c DouniuClient.h GameRule.c GameRule.h DouniuStruct.h msg.h 
	gcc -o $@ $^ -D_DEBUG -lpthread

GameLogicTest: GameLogicTest.c GameLogic.c GameLogic.h GameRule.c GameRule.h
	gcc -o $@ $^

sqliteTest: sqliteTest.c sqliteHelper.c sqliteHelper.h
	gcc -o $@ $^ -lsqlite3 -I/home7/justinwu/lib/include -L/home7/justinwu/lib/lib

CTest: CTest.c
	gcc -o $@ $^

cleanCTest:
	$(RM) CTest

clean:
	$(RM) $(OBJS)

OBJS := DouniuServer DouniuClient
all: $(OBJS)

DouniuServer: DouniuServer.c 
	gcc -o $@ $^ -D_DEBUG -lpthread

DouniuClient: DouniuClient.c
	gcc -o $@ $^ -D_DEBUG -lpthread

clean:
	$(RM) $(OBJS)

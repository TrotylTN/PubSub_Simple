CC = gcc
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread

all: GroupServer Client

GroupServer: groupServer.c groupServer.h pubsub.h
	${CC} -o GroupServer groupServer.c ${LDFLAGS} ${CFLAGS}

Client: client.c client.h pubsub.h
	${CC} -o Client client.c ${LDFLAGS} ${CFLAGS}

clean:
	rm Client GroupServer

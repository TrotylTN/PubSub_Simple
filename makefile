CC = g++
CFLAGS = -D_REENTRANT -std=c++11
LDFLAGS = -lpthread -lnsl

all: GroupServer Client

GroupServer: groupServer.cpp pubsub.h
	${CC} groupServer.cpp groupServer_xdr.c -o GroupServer ${LDFLAGS} ${CFLAGS}

Client: client.cpp pubsub.h
	${CC} client.cpp groupServer_clnt.c groupServer_xdr.c -o Client ${LDFLAGS} ${CFLAGS}

clean:
	rm Client GroupServer

test: test.cpp pubsub.h
	g++ test.cpp -o test

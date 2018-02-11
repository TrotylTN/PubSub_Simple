CC = g++
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread -lnsl

all: GroupServer Client

GroupServer: groupServer.cpp groupServer_proc.c pubsub.h
	${CC} groupServer.cpp groupServer_proc.c groupServer_xdr.c -o GroupServer ${LDFLAGS} ${CFLAGS}

Client: client.cpp pubsub.h
	${CC} client.cpp groupServer_clnt.c groupServer_xdr.c -o Client ${LDFLAGS} ${CFLAGS}

clean:
	rm Client GroupServer

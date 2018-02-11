CC = g++
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread -lnsl

all: GroupServer Client

GroupServer: groupServer_proc.c groupServer.c pubsub.h
	${CC} groupServer_proc.c groupServer.c groupServer_xdr.c -o GroupServer ${LDFLAGS} ${CFLAGS}

Client: client.c client.h pubsub.h
	${CC} -o Client client.c ${LDFLAGS} ${CFLAGS}

clean:
	rm Client GroupServer

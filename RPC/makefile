CC = gcc
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread -lnsl

all: GroupServer Client

GroupServer: groupServer_proc.c groupServer_svc.c pubsub.h
	${CC} groupServer_proc.c groupServer_svc.c groupServer_xdr.c -o GroupServer ${LDFLAGS} ${CFLAGS}

Client: client.c client.h pubsub.h
	${CC} -o Client client.c ${LDFLAGS} ${CFLAGS}

clean:
	rm Client GroupServer

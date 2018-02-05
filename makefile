CC = gcc
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread

all: GroupServer Client RegistryServer

GroupServer: groupServer.c groupServer.h
	${CC} -o GroupServer groupServer.c ${LDFLAGS} ${CFLAGS}

Client: client.c client.h
	${CC} -o Client client.c ${LDFLAGS} ${CFLAGS}

RegistryServer: registryServer.c registryServer.h
	${CC} -o RegistryServer registryServer.c ${LDFLAGS} ${CFLAGS}

clean:
	rm Client GroupServer RegistryServer

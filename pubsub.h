// General headfile for the project

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

#define MAXCLIENT 100
#define MAXSERVER 100
#define MAXSTRING 120
#define MAXSUBSCRIPE 100

using namespace std;

bool sub_article_valid(char* sub_article) {
  // TODO: check if this is a valid subscribe article request
  return true;
}

bool pub_article_valid(char* sub_article) {
  // TODO: check if this is a valid publish article request
  return true;
}

/*
  RPC call return value:
  0: unknow error
  1: executed successfully
  2: duplicate connection request
  3: connection has not established
  4: duplicate subscribe request for a client
  5: reached the MAXSUBSCRIPE for certain client
  6: reached the limit for connected client
  7: unsubscribe a non-existent subscripition
 */

// UDP help function implementation
int UDP_send_packet(char *packet_content, char *dest_IP, unsigned short dest_port) {
  struct sockaddr_in si_other;

}

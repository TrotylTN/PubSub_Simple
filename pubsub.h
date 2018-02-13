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
int UDP_send_packet(const char *packet_content,
                    const char *dest_IP,
                    const unsigned short dest_port) {
  struct sockaddr_in si_other;
  int s; // socket
  int slen=sizeof(si_other);
  if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      perror("socket error");
      close(s);
      return -1;
  }
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(dest_port);

  if (inet_aton(dest_IP , &si_other.sin_addr) == 0)
  {
      fprintf(stderr, "inet_aton() failed\n");
      close(s);
      return -1;
  }
  // send the message
  if (sendto(s, packet_content, strlen(packet_content), 0,
             (struct sockaddr *) &si_other, slen)==-1) {
    perror("sendto failed");
    close(s);
    return -1;
  }
  close(s);
  return 0;
}

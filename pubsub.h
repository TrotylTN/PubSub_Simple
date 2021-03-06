// General headfile for the project

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

#define MAXCLIENT 100
#define MAXSERVER 100
#define MAXSTRING 128
#define MAXSUBSCRIPE 100
#define REG_SERVER "128.101.35.147"
#define REG_PORT 5105

using namespace std;

bool check_type_correctness(const string a) {
  static const string standard_types[10] = {"Sports", "Lifestyle",
                                            "Entertainment", "Business",
                                            "Technology", "Science", "Politics",
                                            "Health"};
  for (int i = 0; i < 8; i ++) {
    if (a == standard_types[i])
      return true;
  }
  return false;
}

bool sub_article_valid(const char* sub_article) {
  int len = strlen(sub_article);
  int semi_cnt = 0;
  for (int i = 0; i < len; i++) {
    if (sub_article[i] == ';')
      semi_cnt ++;
  }
  if (semi_cnt != 3)
  // must contain exactly 3 ';'
    return false;

  int first_semi = 0, second_semi, third_semi;
  while (first_semi < len && sub_article[first_semi] != ';')
    first_semi ++;
  second_semi = first_semi + 1;
  while (second_semi < len && sub_article[second_semi] != ';')
    second_semi ++;
  third_semi = second_semi + 1;
  while (third_semi < len && sub_article[third_semi] != ';')
    third_semi ++;

  int first_seg = first_semi;
  int second_seg = second_semi - first_semi - 1;
  int third_seg = third_semi - second_semi - 1;
  int forth_seg = len - 1 - third_semi;
  if (forth_seg != 0)
  // must be blank to subscribe
    return false;
  if (first_seg == 0 && second_seg == 0 && third_seg == 0)
  // must have something
    return false;
  if (first_seg > 0) {
    char buf[MAXSTRING];
    memset(buf, 0, sizeof buf);
    strncpy(buf, sub_article, first_semi);
    string art_type = string(buf);
    if (!check_type_correctness(art_type))
      return false;
  }
  return true;
}

bool pub_article_valid(const char* sub_article) {
  // TODO: check if this is a valid publish article request
  int len = strlen(sub_article);
  int semi_cnt = 0;
  for (int i = 0; i < len; i++) {
    if (sub_article[i] == ';')
      semi_cnt ++;
  }
  if (semi_cnt < 3)
  // must contain >= 3 ';'
    return false;

  int first_semi = 0, second_semi, third_semi;
  while (first_semi < len && sub_article[first_semi] != ';')
    first_semi ++;
  second_semi = first_semi + 1;
  while (second_semi < len && sub_article[second_semi] != ';')
    second_semi ++;
  third_semi = second_semi + 1;
  while (third_semi < len && sub_article[third_semi] != ';')
    third_semi ++;

  int first_seg = first_semi;
  int second_seg = second_semi - first_semi - 1;
  int third_seg = third_semi - second_semi - 1;
  int forth_seg = len - 1 - third_semi;
  if (forth_seg == 0)
  // must have something to publish
    return false;
  if (first_seg == 0 && second_seg == 0 && third_seg == 0)
  // must have something to tag
    return false;
  if (first_seg > 0) {
    char buf[MAXSTRING];
    memset(buf, 0, sizeof buf);
    strncpy(buf, sub_article, first_semi);
    string art_type = string(buf);
    if (!check_type_correctness(art_type))
      return false;
  }
  return true;
}

string retrieve_type(const char *sub_article) {
  static string res;
  int first_semi = 0;
  int len = strlen(sub_article);
  char buf[MAXSTRING];
  memset(buf, 0, sizeof buf);
  while (first_semi < len && sub_article[first_semi] != ';')
    first_semi ++;
  strncpy(buf, sub_article, first_semi);
  buf[first_semi] = 0;
  res = string(buf);
  return res;
}

string retrieve_ogitor(const char *sub_article) {
  static string res;
  int first_semi = 0, second_semi;
  int len = strlen(sub_article);
  char buf[MAXSTRING];
  memset(buf, 0, sizeof buf);
  while (first_semi < len && sub_article[first_semi] != ';')
    first_semi ++;
  second_semi = first_semi + 1;
  while (second_semi < len && sub_article[second_semi] != ';')
    second_semi ++;
  len = second_semi - first_semi - 1;
  strncpy(buf, sub_article + first_semi + 1, len);
  buf[len] = 0;
  res = string(buf);
  return res;
}

string retrieve_org(const char *sub_article) {
  static string res;
  int first_semi = 0, second_semi, third_semi;
  int len = strlen(sub_article);
  char buf[MAXSTRING];
  memset(buf, 0, sizeof buf);
  while (first_semi < len && sub_article[first_semi] != ';')
    first_semi ++;
  second_semi = first_semi + 1;
  while (second_semi < len && sub_article[second_semi] != ';')
    second_semi ++;
  third_semi = second_semi + 1;
  while (third_semi < len && sub_article[third_semi] != ';')
    third_semi ++;
  len = third_semi - second_semi - 1;
  strncpy(buf, sub_article + second_semi + 1, len);
  buf[len] = 0;
  res = string(buf);
  return res;
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
  8: invalid subscribe or publish request
 */

void RPC_error_no(int n) {
  if (n == 2)
    puts("ERROR: duplicate connection request");
  else if (n == 3)
    puts("ERROR: connection has not established");
  else if (n == 4)
    puts("ERROR: duplicate subscribe request for a client");
  else if (n == 5)
    puts("ERROR: reached the MAXSUBSCRIPE for certain client");
  else if (n == 6)
    puts("ERROR: reached the limit for connected client");
  else if (n == 7)
    puts("ERROR: unsubscribe a non-existent subscripition");
  else if (n == 8)
    puts("ERROR: invalid subscribe or publish request");
  else
    puts("ERROR: unknow error");
}

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

// UDP help function implementation
int UDP_send_packet_socket(const char *packet_content,
                    const char *dest_IP,
                    const unsigned short dest_port,
                    int s) {
  struct sockaddr_in si_other;
  int slen=sizeof(si_other);

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(dest_port);

  if (inet_aton(dest_IP , &si_other.sin_addr) == 0)
  {
      fprintf(stderr, "inet_aton() failed\n");
      return -1;
  }
  // send the message
  if (sendto(s, packet_content, strlen(packet_content), 0,
             (struct sockaddr *) &si_other, slen)==-1) {
    perror("sendto failed");
    return -1;
  }
  return 0;
}


// get local ip address
string get_local_IP(void) {
  struct ifaddrs *ifa=NULL, *i=NULL;
  getifaddrs(&ifa);
  string local_ip = "";
  for (i = ifa; i != NULL; i = i->ifa_next) {
    if (!i->ifa_addr)
        continue;
    if (i->ifa_addr->sa_family == AF_INET) {
      char buf[INET_ADDRSTRLEN];
      inet_ntop(
        AF_INET,
        &((struct sockaddr_in *)i->ifa_addr)->sin_addr,
        buf,
        INET_ADDRSTRLEN
      );
      if (string(buf) != "127.0.0.1") {
        local_ip = string(buf);
        break;
      }
    }
  }
  if (ifa != NULL) freeifaddrs(ifa);
  return local_ip;
}

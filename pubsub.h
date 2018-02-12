// General headfile for the project

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAXCLIENT 100
#define MAXSERVER 100
#define MAXSTRING 120

bool sub_article_valid(char* sub_article) {
  // TODO: check if this is a valid subscribe article request
  return true;
}

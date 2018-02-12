// General headfile for the project

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
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

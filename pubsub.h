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

int article_index(pair<string, int> unique_id,
                  string article_sub,
                  map<pair<string, int>, vector<string> > &client_subinfo){
  // look for the index for this article_sub in this map

  return -1;
}

/*
  RPC call return value:
  1: executed successfully
  2: duplicate connection request
  3: connection has not established
  4: duplicate subscribe request for a client
  5: reached the MAXSUBSCRIPE for certain client
  6: reached the limit for connected client
 */

#include <cstdio>
#include <iostream>
#include "groupServer.h"

using namespace std;

int main() {
  CLIENT *clnt;
  int *result;
  char server_addr[128];
  int UDP_port_num;

  bool server_joined = false;
  printf("Please enter the port # you would like to receive article: ");
  cin >> UDP_port_num;
  // TODO: create a thread to bind the port to receive UDP packet

  // main thread
  while (true) {
    if (!server_joined) {
      // has not joined server
      printf("You have not connected to a server\n");
      printf("Please enter the server's IP address: ");
      scanf("%s", server_addr);

      clnt = clnt_create(server_addr, PUBSUBGROUP, PUBSUBGROUP_VER, "visible");

      if (clnt == (CLIENT *)NULL) {
        // Couldn't establish connection
        clnt_pcreateerror(server_addr);
        server_joined = false;
        printf("Connection cannot be established.\n");
        continue;
      }
      result = join_1(server_addr, UDP_port_num, clnt);

      if (result == (int *)NULL) {
        /*
         * An error occurred while calling
         * the server.
         */
        clnt_perror(clnt, server_addr);
        server_joined = false;
        printf("An error occurred while calling the server\n");
        continue;
      }
      // successfully called the RPC function
      if (*result != 1) {
        // unable to process your request
        printf("The server cannot process your request\n");
        server_joined = false;
        continue;
      }

      printf("You have successfully connected to server %s\n", server_addr);
      server_joined = true;

    } else {
      // joined a server, support other operation
    }
  }


  return 0;
}

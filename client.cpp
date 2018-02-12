#include <cstdio>
#include <iostream>
#include "groupServer.h"

using namespace std;

int main() {
  CLIENT *clnt;
  int *result;
  char server_addr[128];
  int UDP_port_num;

  // Initially, connection status is FALSE
  bool server_joined = false;

  printf("Please enter the port # you would like to receive article: ");
  cin >> UDP_port_num;
  // TODO: create a thread to bind the port to receive UDP packet

  // main thread
  while (true) {
    printf("\n");
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
        printf("An error occurred while calling the server\n");
        continue;
      }
      // successfully called the RPC function
      if (*result != 1) {
        // unable to process your request
        printf("The server cannot process your request, try again later.\n");
        continue;
      }

      server_joined = true;
      printf("You have successfully connected to server %s\n", server_addr);
    } else {
      // joined a server, support other operation
      string op_id;
      printf("You have connected to the server %s\n", server_addr);
      printf("  0.Ping\n  1. Leave\n  2.subscribe\n  3. unsubscribe\n");
      printf("  4.Publish\n  9.Exit\n");
      printf("Please enter the operation #: \n");
      cin >> op_id;
      if (op_id == "1") {
        // Leave the server
        
      } else if (op_id == "2") {
        // Subscribe some articles
      } else if (op_id == "3") {
        // Unsubscribe some article
      } else if (op_id == "4") {
        // Publish an article
      } else if (op_id == "0") {
        // Ping only
      } else if (op_id == "9") {
        // Leave the server and Exit
      } else {
        printf("invalid operation #, try again.\n");
      }
    }
  }


  return 0;
}

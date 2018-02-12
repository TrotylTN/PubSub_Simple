#include <cstdio>
#include <iostream>
#include "groupServer.h"
#include "pubsub.h"

using namespace std;

int main() {
  CLIENT *clnt;
  int *result;
  char server_addr[32];
  char self_addr[32];
  char article_cat[128];
  int UDP_port_num;

  // Initially, connection status is FALSE
  bool server_joined = false;

  printf("Please enter the port # you would like to receive article: ");
  cin >> UDP_port_num;
  // TODO: create a thread to bind the port to receive UDP packet

  // main thread
  while (true) {
    printf("\n");
    printf("----------------------------------------\n");
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

      result = join_1(self_addr, UDP_port_num, clnt);

      if (result == (int *)NULL) {
        /*
         * An error occurred while calling
         * the server.
         */
        clnt_perror(clnt, server_addr);
        printf("An error occurred while calling the server, try again later.\n");
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
      printf("\nYou have connected to the server %s\n", server_addr);
      printf("  0. Ping\n  1. Leave\n  2. Subscribe\n  3. Unsubscribe\n");
      printf("  4. Publish\n  9. Exit\n");
      printf("Please enter the operation #: ");
      cin >> op_id;
      if (op_id == "1") {
        // Leave the server
        result = leave_1(self_addr, UDP_port_num, clnt);
        if (result == (int *)NULL) {
          /*
           * An error occurred while calling
           * the server.
           */
          clnt_perror(clnt, server_addr);
          printf("An error occurred while calling the server, try again later.\n");
          continue;
        }
        // successfully called the RPC function
        if (*result != 1) {
          // unable to process your request
          printf("The server cannot process your request, try again later.\n");
          continue;
        }
        // Destroy the client
        clnt_destroy(clnt);
        printf("You have successfully leaved the server %s\n", server_addr);
        server_joined = false;

      } else if (op_id == "2") {
        // Subscribe some articles

        printf("Please enter the article as the form below\n");
        printf("  type;originator;org;contents(must be blank for subscribe)\n");
        printf("  Note: type must be one of <Sports, Lifestyle, Entertainment, Business, Technology, Science, Politics, Health>\n");
        printf("Enter the article you want to subscribe: ");
        scanf("%s", article_cat);
        if (sub_article_valid(article_cat) == false) {
          printf("ERROR: Your input article subscribe request is invalid\n");
          continue;
        }
        result = subscribe_1(self_addr, UDP_port_num, article_cat, clnt);
        if (result == (int *)NULL) {
          /*
           * An error occurred while calling
           * the server.
           */
          clnt_perror(clnt, server_addr);
          printf("An error occurred while calling the server, try again later.\n");
          continue;
        }
        // successfully called the RPC function
        if (*result != 1) {
          // unable to process your request
          printf("The server cannot process your request, try again later.\n");
          continue;
        }


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

#include <cstdio>
#include <iostream>
#include "groupServer.h"
#include "pubsub.h"

void * receiving_article(void *arg) {
	int port_num = *((int *) arg);
	struct sockaddr_in si_me, si_other;

	int s, i, slen = sizeof(si_other) , recv_len;
	char buf[512];
	char dest_IP[32];
	memset(buf, 0, sizeof buf);
	memset(dest_IP, 0, sizeof dest_IP);
	int dest_port;
	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("creating socket failed");
		return NULL;
	}
	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port_num);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) {
		perror("bind error");
		return NULL;
	}
	// listening for data
	while (1) {
		printf("start receiving articles by Port [%d]\n", port_num);
    fflush(stdout);
		memset(buf, '\0', sizeof(buf));

		socklen_t socketlen = slen;
		if ((recv_len = recvfrom(s, buf, 512, 0, (struct sockaddr *) &si_other, &socketlen)) == -1) {
				perror("recfrom error");
				return NULL;
		}
		strncpy(dest_IP, inet_ntoa(si_other.sin_addr), 32);
		dest_port = ntohs(si_other.sin_port);
		printf("\n\n!!!ARTICLE!!!: received article <%s> from %s:%d\n\n", buf, dest_IP, dest_port);
	}
	return NULL;
}


int main() {
  CLIENT *clnt;
  int *result;
  char server_addr[32];
  char self_addr[32];
  char article_cat[MAXSTRING];
	memset(server_addr, 0, sizeof server_addr);
	memset(self_addr, 0, sizeof self_addr);
	memset(article_cat, 0, sizeof article_cat);
  int UDP_port_num;
  pthread_t t_receive_article;
  // Initially, connection status is FALSE
  bool server_joined = false;

  string self_addr_in_string = get_local_IP();
  strncpy(self_addr, self_addr_in_string.c_str(), 32);
  printf("Local IP Address is %s\n", self_addr);

  printf("Please enter the port # you would like to receive article: ");
  cin >> UDP_port_num;
  // create a thread to bind the port to receive UDP packet for article
	pthread_create(
		&t_receive_article,
		NULL,
		receiving_article,
		(void *) &UDP_port_num
	);
  // main thread
	sleep(1);
  while (true) {
    printf("\n");
    printf("----------------------------------------\n");
    if (!server_joined) {
      // has not joined server
      printf("You have not connected to a server\n");
      printf("Please enter the server's IP address: ");
      scanf("%s", server_addr);

      clnt = clnt_create(server_addr, PUBSUBGROUP, PUBSUBGROUP_VER, "tcp");

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
				RPC_error_no(*result);
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
					RPC_error_no(*result);
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
        printf("  type;originator;org;contents(contents must be blank for subscribe)\n");
        printf("  Note: type must be one of <Sports, Lifestyle, Entertainment, Business, Technology, Science, Politics, Health>\n");
        printf("Enter the article you want to subscribe: ");
        gets(article_cat);
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
					RPC_error_no(*result);
          // unable to process your request
          printf("The server cannot process your request, try again later.\n");
          continue;
        }

        // successfully subscribed
        printf("Your subscribe request {%s} has been sent successfully\n", article_cat);

      } else if (op_id == "3") {
        // Unsubscribe some article

        printf("Please enter the article as the form below\n");
        printf("  type;originator;org;contents(contents must be blank for unsubscribe)\n");
        printf("  Note: type must be one of <Sports, Lifestyle, Entertainment, Business, Technology, Science, Politics, Health>\n");
        printf("Enter the article you want to unsubscribe: ");
        gets(article_cat);
        if (sub_article_valid(article_cat) == false) {
          printf("ERROR: Your article unsubscribe request is invalid\n");
          continue;
        }
        result = unsubscribe_1(self_addr, UDP_port_num, article_cat, clnt);
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
					RPC_error_no(*result);
          printf("The server cannot process your request, try again later.\n");
          continue;
        }

        // successfully unsubscribed
        printf("Your unsubscribe request {%s} has been sent successfully\n", article_cat);

      } else if (op_id == "4") {
        // Publish an article
        printf("Please enter the article as the form below\n");
        printf("  type;originator;org;contents\n");
        printf("  Note: type must be one of <Sports, Lifestyle, Entertainment, Business, Technology, Science, Politics, Health>\n");
        printf("  For publish purpose, contents must have something and at least one of first three fields is not blank\n");
        printf("Enter the article you want to publish: ");
        gets(article_cat);
        if (pub_article_valid(article_cat) == false) {
          printf("ERROR: Your article publish request is invalid\n");
          continue;
        }

        result = publish_1(article_cat, self_addr, UDP_port_num, clnt);

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
					RPC_error_no(*result);
          printf("The server cannot process your request, try again later.\n");
          continue;
        }

        // successfully published
        printf("Your publish request {%s} has been sent successfully\n", article_cat);
      } else if (op_id == "0") {
        // Ping only
        result = ping_1(clnt);
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
					RPC_error_no(*result);
          printf("The server cannot process your request, try again later.\n");
          continue;
        }
        printf("Pinged successfully\n");

      } else if (op_id == "9") {
        // Leave the server and Exit
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
					RPC_error_no(*result);
          printf("The server cannot process your request, try again later.\n");
          continue;
        }
        // Destroy the client
        clnt_destroy(clnt);
        printf("You have successfully leaved the server %s\n", server_addr);
        server_joined = false;
        break;
      } else {
        // not valid operation #
        printf("invalid operation #, try again.\n");
      }
    }
  }
  printf("Client has been terminated.");
  return 0;
}

/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "pubsub.h"
#include "groupServer.h"

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static int *
_join_1 (join_1_argument *argp, struct svc_req *rqstp)
{
	return (join_1_svc(argp->arg1, argp->arg2, rqstp));
}

static int *
_leave_1 (leave_1_argument *argp, struct svc_req *rqstp)
{
	return (leave_1_svc(argp->arg1, argp->arg2, rqstp));
}

static int *
_subscribe_1 (subscribe_1_argument *argp, struct svc_req *rqstp)
{
	return (subscribe_1_svc(argp->arg1, argp->arg2, argp->arg3, rqstp));
}

static int *
_unsubscribe_1 (unsubscribe_1_argument *argp, struct svc_req *rqstp)
{
	return (unsubscribe_1_svc(argp->arg1, argp->arg2, argp->arg3, rqstp));
}

static int *
_publish_1 (publish_1_argument *argp, struct svc_req *rqstp)
{
	return (publish_1_svc(argp->arg1, argp->arg2, argp->arg3, rqstp));
}

static int *
_ping_1 (void  *argp, struct svc_req *rqstp)
{
	return (ping_1_svc(rqstp));
}

static void
pubsubgroup_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		join_1_argument join_1_arg;
		leave_1_argument leave_1_arg;
		subscribe_1_argument subscribe_1_arg;
		unsubscribe_1_argument unsubscribe_1_arg;
		publish_1_argument publish_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case JOIN:
		_xdr_argument = (xdrproc_t) xdr_join_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _join_1;
		break;

	case LEAVE:
		_xdr_argument = (xdrproc_t) xdr_leave_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _leave_1;
		break;

	case SUBSCRIBE:
		_xdr_argument = (xdrproc_t) xdr_subscribe_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _subscribe_1;
		break;

	case UNSUBSCRIBE:
		_xdr_argument = (xdrproc_t) xdr_unsubscribe_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _unsubscribe_1;
		break;

	case PUBLISH:
		_xdr_argument = (xdrproc_t) xdr_publish_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _publish_1;
		break;

	case PING:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _ping_1;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

// definition for global variable
static pthread_mutex_t client_operation_lock = PTHREAD_MUTEX_INITIALIZER;
static int total_client = 0;
static map< pair<string, int>, vector<string> > client_subinfo;
static map< pair<string, int>, bool> client_connection;
static vector<pair<string, int> > connected_clients;
static string self_IP;
static int port_num;
// Group Server receiving heartbeat from registery server
void * hearing_heartbeat(void *arg) {
	int port_num = *((int *) arg);
	struct sockaddr_in si_me, si_other;

	int s, i, slen = sizeof(si_other) , recv_len;
	char buf[512];
	char dest_IP[32];
	memset(buf, 0, sizeof buf);
	memset(dest_IP, 0, sizeof buf);
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
		printf("Server start receiving heartbeat by Port [%d]\n", port_num);
    fflush(stdout);
		memset(buf, '\0', sizeof(buf));

		socklen_t socketlen = slen;
		if ((recv_len = recvfrom(s, buf, 512, 0, (struct sockaddr *) &si_other, &socketlen)) == -1) {
				perror("recfrom error");
				return NULL;
		}
		strncpy(dest_IP, inet_ntoa(si_other.sin_addr), 32);
		dest_port = ntohs(si_other.sin_port);
		printf("received \"%s\" from %s:%d\n", buf, dest_IP, dest_port);

		// reply
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
		{
			perror("replying error");
			return NULL;
		}
	}
	return NULL;
}

// Lookup the index for the subscripition
int get_article_index(const pair<string, int> unique_id,
                  const string article_sub){
  // look for the index for this article_sub in this map
	for (int i = 0; i < client_subinfo[unique_id].size(); i ++) {
		if (client_subinfo[unique_id][i] == article_sub)
			return i;
	}
  return -1;
}

// Lookup the index for the clients
int get_client_index(const pair<string, int> unique_id) {
	for (int i = 0; i < connected_clients.size(); i++) {
		if (connected_clients[i] == unique_id)
			return i;
	}
	return -1;
}

// check whether this client has subscribed the
bool subscripition_match(pair<string, int> unique_id, char *article_pub) {
	int cur_seg_len;
	string subscripition_seg;
	bool if_sending = false;
	for (int i = 0; i < client_subinfo[unique_id].size(); i ++) {
		if_sending = true;
		// first element
		subscripition_seg = retrieve_type(client_subinfo[unique_id][i].c_str());
		if (subscripition_seg.length() != 0) {
			if (subscripition_seg != retrieve_type(article_pub))
				if_sending = false;
		}
		// second element
		subscripition_seg = retrieve_ogitor(client_subinfo[unique_id][i].c_str());
		if (subscripition_seg.length() != 0) {
			if (subscripition_seg != retrieve_ogitor(article_pub))
				if_sending = false;
		}
		// third element
		subscripition_seg = retrieve_org(client_subinfo[unique_id][i].c_str());
		if (subscripition_seg.length() != 0) {
			if (subscripition_seg != retrieve_org(article_pub))
				if_sending = false;
		}
		if (if_sending)
		// if all clients' required fields have been filled, return true
			return true;
	}
	return false;
}

// publish article to subscribed
void publish_to_subscriped_clients(char *article_pub) {
  for (int i = 0; i < connected_clients.size(); i++) {
		if (subscripition_match(connected_clients[i], article_pub)) {
			string dest_IP = connected_clients[i].first;
			int port_num = connected_clients[i].second;
			if (UDP_send_packet(article_pub, dest_IP.c_str(), port_num) == -1) {
				printf("  Tried to send article to %s:%d but failed\n", dest_IP.c_str(), port_num);
			} else {
				printf("  Sent <%s> to %s:%d\n", article_pub, dest_IP.c_str(), port_num);
			}
		}
	}
}

// implementation for RPC functions for clients' usage
int * join_1_svc(char * ip_addr, int port_num, struct svc_req *req) {
  static int result;
	pthread_mutex_lock(&client_operation_lock);
	result = 0;
	string client_ip = string(ip_addr);
	if (client_connection[make_pair(client_ip, port_num)] == false) {
		if (total_client < MAXCLIENT) {
			client_connection[make_pair(client_ip, port_num)] = true;
			client_subinfo[make_pair(client_ip, port_num)].clear();
			printf("Client %s:%d connected to the server\n", ip_addr, port_num);
			total_client++;
			// insert this client into the list
			connected_clients.push_back(make_pair(client_ip, port_num));
			// successfully registered this client
		  result = 1;
		} else {
			client_connection[make_pair(client_ip, port_num)] = true;
			client_subinfo[make_pair(client_ip, port_num)].clear();
			printf("Client %s:%d tried to connect to the server, but the number of connected clients has reached the limit %d\n", ip_addr, port_num, MAXCLIENT);
			// error # 6 reached the limit for connected client
			result = 6;
		}
	} else {
		// This server have already resgitered
		printf("Client %s:%d tried to connect to the server, but it has already connected\n", ip_addr, port_num);
		// error # 2: duplicate connection request
		result = 2;
		// have no need to do anything, just continue
	}
	pthread_mutex_unlock(&client_operation_lock);
  return (&result);
}

int * leave_1_svc(char * ip_addr, int port_num, struct svc_req *req) {
  static int result;
	pthread_mutex_lock(&client_operation_lock);
	result = 0;
	string client_ip = string(ip_addr);
	if (client_connection[make_pair(client_ip, port_num)] == true) {
		// start to disconnect
		client_connection[make_pair(client_ip, port_num)] = false;
		client_subinfo[make_pair(client_ip, port_num)].clear();
		printf("Client %s:%d left the server, clean all cache for it\n", ip_addr, port_num);
		total_client--;
		int client_idx = get_client_index(make_pair(client_ip, port_num));
		if (client_idx == -1) {
			printf("error while deleting a client\n");
		} else {
			// remove this client from the list
			connected_clients.erase(connected_clients.begin() + client_idx);
		}
		// successfully left
		result = 1;
	} else {
		// no connection found
		client_connection[make_pair(client_ip, port_num)] = false;
		client_subinfo[make_pair(client_ip, port_num)].clear();
		printf("Client %s:%d tried to leave the server, but it has not connected yet.\n", ip_addr, port_num);
		// error # 3, connection has not established
		result = 3;
	}
	pthread_mutex_unlock(&client_operation_lock);
  return (&result);
}

int * subscribe_1_svc(char * ip_addr, int port_num, char * article,
                      struct svc_req *req) {
  static int result;
	pthread_mutex_lock(&client_operation_lock);
	result = 0;
	string client_ip = string(ip_addr);
	string article_sub = string(article);
	if (client_connection[make_pair(client_ip, port_num)] == true) {
		// connected
		if (get_article_index(make_pair(client_ip, port_num), article_sub) == -1) {
			if (client_subinfo[make_pair(client_ip, port_num)].size() < MAXSUBSCRIPE) {
				if (sub_article_valid(article)) {
					// Here is validing the subscribe request
					client_subinfo[make_pair(client_ip, port_num)].push_back(article_sub);
					// successfully subscribed
					printf("Client %s:%d subscribed <%s>.\n", ip_addr, port_num, article);
					result = 1;
				} else {
					printf("Client %s:%d tried to subscribe <%s>, but the request is invalid\n", ip_addr, port_num, article);
					// error # 8: invalid subscribe or publish request
					result = 8;
				}
			} else {
				printf("Client %s:%d tried to subscribe <%s>, but it reached the max subscribe number %d.\n", ip_addr, port_num, article, MAXSUBSCRIPE);
				// error # 5: reached the MAXSUBSCRIPE for certain client
				result = 5;
			}
		} else {
			printf("Client %s:%d tried to subscribe <%s>, but it's a duplicate subscribe.\n", ip_addr, port_num, article);
			// error # 4: duplicate subscribe for a client
			result = 4;
		}
	} else {
		client_connection[make_pair(client_ip, port_num)] = false;
		client_subinfo[make_pair(client_ip, port_num)].clear();
		printf("Client %s:%d tried to subscribe, but it has not connected yet.\n", ip_addr, port_num);
		// error # 3, connection has not established
		result = 3;
	}
	pthread_mutex_unlock(&client_operation_lock);
  return (&result);
}

int * unsubscribe_1_svc(char * ip_addr, int port_num, char * article,
                        struct svc_req *req) {
  static int result;
	pthread_mutex_lock(&client_operation_lock);
	result = 0;
	string client_ip = string(ip_addr);
	string article_sub = string(article);
	int sub_index = -1;
	if (client_connection[make_pair(client_ip, port_num)] == true) {
		// connected
		if ((sub_index = get_article_index(make_pair(client_ip, port_num),
																	 		 article_sub)) != -1) {
			// start to unsubscribe
			client_subinfo[make_pair(client_ip, port_num)].erase(
				client_subinfo[make_pair(client_ip, port_num)].begin() + sub_index
			);
			printf("Client %s:%d unsubscribed <%s>.\n", ip_addr, port_num, article);
			// successfully executed
			result = 1;
		} else {
			printf("Client %s:%d tried to unsubscribe <%s>, but this subscripition is not existent.\n", ip_addr, port_num, article);
			// error # 7: unsubscribe a non-existent subscripition
			result = 7;
		}
	} else {
		client_connection[make_pair(client_ip, port_num)] = false;
		client_subinfo[make_pair(client_ip, port_num)].clear();
		printf("Client %s:%d tried to unsubscribe, but it has not connected yet.\n", ip_addr, port_num);
		// error # 3, connection has not established
		result = 3;
	}
	pthread_mutex_unlock(&client_operation_lock);
  result = 1;
  return (&result);
}

int * publish_1_svc(char * article, char * ip_addr, int port_num,
                    struct svc_req *req) {
  static int result;
	pthread_mutex_lock(&client_operation_lock);
	string client_ip = string(ip_addr);
	result = 0;
	// publish to all available
	if (client_connection[make_pair(client_ip, port_num)] == true) {
		// connected
		if (pub_article_valid(article)) {
			// valid the publish request
			publish_to_subscriped_clients(article);
			printf("Client %s:%d published <%s>\n", ip_addr, port_num, article);
			// successfully executed
			result = 1;
		} else {
			printf("Client %s:%d tried to publish <%s>, but the request is invalid\n", ip_addr, port_num, article);
			// error # 8: invalid subscribe or publish request
			result = 8;
		}
	} else {
			client_connection[make_pair(client_ip, port_num)] = false;
			client_subinfo[make_pair(client_ip, port_num)].clear();
			printf("Client %s:%d tried to publish <%s>, but it has not connected yet.\n", ip_addr, port_num, article);
			// error # 3, connection has not established
			result = 3;
	}
 	pthread_mutex_unlock(&client_operation_lock);
  return (&result);
}

int * ping_1_svc(struct svc_req *req) {
	// just ping, return successfully executed number
  static int result;
  result = 1;
  return (&result);
}

void Register(string self_IP, int self_port) {
	char buf[1024];
	memset(buf, 0, sizeof buf);
	string combined_str;
	// ​[“Register;RPC;IP;Port;ProgramID;Version”]
	combined_str = "Register;RPC;" + self_IP + ";" +
								 to_string(self_port) + ";0x20000001;1";
	strncpy(buf, combined_str.c_str(), 1024);
	// The registry server will be on dio.cs.umn.edu ("128.101.35.147") with port 5105
	UDP_send_packet(buf, REG_SERVER, REG_PORT);
	return;
}

void Deregister(string self_IP, int self_port) {
	char buf[1024];
	memset(buf, 0, sizeof buf);
	string combined_str;
	// ​[“Deregister;RPC;IP;Port”]
	combined_str = "Deregister;RPC;" + self_IP + ";" + to_string(self_port);
	strncpy(buf, combined_str.c_str(), 1024);
	// The registry server will be on dio.cs.umn.edu ("128.101.35.147") with port 5105
	UDP_send_packet(buf, REG_SERVER, REG_PORT);
	return;
}

string GetList(string self_IP, int self_port) {
	// TODO
	char buf[1024];
	memset(buf, 0, sizeof buf);
	string combined_str;
	// [“GetList;RPC;IP;Port”]
	combined_str = "GetList;RPC;" + self_IP + ";" + to_string(self_port);
	strncpy(buf, combined_str.c_str(), 1024);
	// The registry server will be on dio.cs.umn.edu ("128.101.35.147") with port 5105
	char dest_IP[32] = REG_SERVER;
	unsigned short dest_port = REG_PORT;
	string result = "";

	struct sockaddr_in si_other;
	int s; // socket
	int slen=sizeof(si_other);
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
			perror("socket error");
			close(s);
			return "";
	}
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(dest_port);

	if (inet_aton(dest_IP , &si_other.sin_addr) == 0)
	{
			fprintf(stderr, "inet_aton() failed\n");
			close(s);
			return "";
	}
	// send the message
	if (sendto(s, buf, strlen(buf), 0,
						 (struct sockaddr *) &si_other, slen)==-1) {
		perror("sendto failed");
		close(s);
		return "";
	}
	printf("GetList(%s) request sent.", buf);

	// receiving data
	memset(buf, 0, sizeof buf);
	socklen_t socketlen = slen;
	if (recvfrom(s, buf, 1024, 0, (struct sockaddr *) &si_other, &socketlen) == -1)
	{
			perror("recvfrom()");
			close(s);
			return "";
	}
	result = string(buf);
	printf("return value for GetList() is: [%s]\n", result.c_str());
	close(s);
	return result;
}

void * listen_to_cmd(void *arg) {
	int port_num = *((int *) arg);
	while (true) {
		char op[256];
		printf("Please enter the number to control the server.\n");
		printf("  1. GetList()\n  2. Deregister() -- Close the server\nEnter your choice: ");
		scanf("%s", op);
		if (string(op) == "1")
			GetList(self_IP, port_num);
		else if (string(op) == "2") {
			Deregister(self_IP, port_num);
			exit(0);
		}
		else
			printf("invalid operation. Try again\n\n");
	}
	return NULL;
}

void stop_server(int signo) {
	printf("\nBeing enforced exiting...\n");
	Deregister(self_IP, port_num);
	printf("Deregistered this server\n");
  exit(0);
}

int
main (int argc, char **argv)
{
	struct sigaction act;
	act.sa_handler = stop_server;
	sigfillset(&act.sa_mask);

	self_IP = get_local_IP();
	pthread_t t_heartbeat;
	pthread_t t_listen_cmd;
	printf("Enter the port you would like to listen to the server's heartbeat:");
	cin >> port_num;

	// Register the server
	Register(self_IP, port_num);
	sigaction(SIGINT, &act, NULL);

	pthread_create(&t_heartbeat, NULL, hearing_heartbeat, (void *) &port_num);
	pthread_create(&t_listen_cmd, NULL, listen_to_cmd, (void *) &port_num);
	// reset all connection
	client_connection.clear();
	client_subinfo.clear();
	connected_clients.clear();
	// server starts
	printf("Server running...\n");
	// RPC Initialization
	register SVCXPRT *transp;

	pmap_unset (PUBSUBGROUP, PUBSUBGROUP_VER);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PUBSUBGROUP, PUBSUBGROUP_VER, pubsubgroup_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PUBSUBGROUP, PUBSUBGROUP_VER, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PUBSUBGROUP, PUBSUBGROUP_VER, pubsubgroup_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PUBSUBGROUP, PUBSUBGROUP_VER, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}

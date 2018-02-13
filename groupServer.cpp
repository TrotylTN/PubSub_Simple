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
	// TODO: to implement the match 
	return false;
}

// publish article to subscribed
void publish_to_subscriped_clients(char *article_pub) {
  for (int i = 0; i < connected_clients.size(); i++) {
		if (subscripition_match(connected_clients[i], article_pub)) {
			string dest_IP = connected_clients[i].first;
			int port_num = connected_clients[i].second;
			if (UDP_send_packet(article_pub, dest_IP.c_str(), port_num) == -1) {
				printf("Tried to send article to %s:%d but failed\n", dest_IP.c_str(), port_num);
			}
		}
	}
}

// implementation for RPC functions for clients' usage
int * join_1_svc(char * ip_addr, int port_num, struct svc_req *req) {
  static int result;
	result = 0;
	string client_ip = string(ip_addr);
	pthread_mutex_lock(&client_operation_lock);
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
	result = 0;
	string client_ip = string(ip_addr);
	pthread_mutex_lock(&client_operation_lock);
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
	result = 0;
	string client_ip = string(ip_addr);
	string article_sub = string(article);
	pthread_mutex_lock(&client_operation_lock);
	if (client_connection[make_pair(client_ip, port_num)] == true) {
		// connected
		if (get_article_index(make_pair(client_ip, port_num), article_sub) == -1) {
			if (client_subinfo[make_pair(client_ip, port_num)].size() < MAXSUBSCRIPE) {
				client_subinfo[make_pair(client_ip, port_num)].push_back(article_sub);
				// successfully subscribed
				printf("Client %s:%d subscribed <%s>.\n", ip_addr, port_num, article);
				result = 1;
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
	result = 0;
	string client_ip = string(ip_addr);
	string article_sub = string(article);
	int sub_index = -1;
	pthread_mutex_lock(&client_operation_lock);
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
	result = 0;
	pthread_mutex_lock(&client_operation_lock);
	// publish to all available
	publish_to_subscriped_clients(article);

 	pthread_mutex_unlock(&client_operation_lock);
  result = 1;
  return (&result);
}

int * ping_1_svc(struct svc_req *req) {
	// just ping, return successfully executed number
  static int result;
  result = 1;
  return (&result);
}


int
main (int argc, char **argv)
{
	// reset all connection
	client_connection.clear();
	client_subinfo.clear();
	connected_clients.clear();

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

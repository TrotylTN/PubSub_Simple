/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "groupServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

// implementation for RPC functions for clients' usage
int * join_1_svc(char * ip_addr, int port_num, struct svc_req *req) {
  static int result;

  result = 1;
  return (&result);
}

int * leave_1_svc(char * ip_addr, int port_num, struct svc_req *req) {
  static int result;

  result = 1;
  return (&result);
}

int * subscribe_1_svc(char * ip_addr, int port_num, char * artcile,
                      struct svc_req *req) {
  static int result;

  result = 1;
  return (&result);
}

int * unsubscribe_1_svc(char * ip_addr, int port_num, char * article,
                        struct svc_req *req) {
  static int result;

  result = 1;
  return (&result);
}

int * publish_1_svc(char * article, char * ip_addr, int port_num,
                    struct svc_req *req) {
   static int result;

   result = 1;
   return (&result);
}

int * ping_1_svc(struct svc_req *req) {
  static int result;
  result = 1;
  return (&result);
}


int
main (int argc, char **argv)
{
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

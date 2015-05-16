/*
 * servFunctions.h
 *
 *  Created on: Apr 4, 2015
 *      Author: keinsword
 */

#ifndef SERVFUNCTIONS_H_
#define SERVFUNCTIONS_H_

#include <sys/types.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <libconfig.h>
#include "../../../nas-srv-et-lib/include/nas-srv-et-lib.h"
#include "netns-func.h"

#define EPOLL_QUEUE_LEN 100
#define MAX_EPOLL_EVENTS 100
#define EPOLL_RUN_TIMEOUT 5

#define MAX_NUM_OF_SERVICES 5
#define MAX_IP_NUM_IN_SET 10

typedef struct {
	const char *srv_name;
	char netns_name[5];
	int netns_fd;
	const char *ip_set[MAX_IP_NUM_IN_SET];
	int ip_in_set;
} service_info;

extern service_info srvInfoTable[MAX_NUM_OF_SERVICES];
extern int num_of_services;

extern int endEventLoop, endMainLoop, epollFD, readyFDs;

extern struct epoll_event event;
extern struct epoll_event evList[MAX_EPOLL_EVENTS];

extern config servConfig;

extern struct sockaddr_in clientAddr;
extern socklen_t clientAddrSize;

int readConfigFile(config_t *cfg);

int checkArgs();

void timeoutCheck(connection *connList);

int createServerSocket();

int acceptNewConnection(int listeningSocket, connection *connList, struct epoll_event *evListItem);

int identifySenderTCP(connection *connList, struct epoll_event *evListItem);

int identifySenderUDP(connection *connList, char *buffer);

int serverChecksumCalculateAndCompare(connection *connListItem, char *buffer);

int recvMessageTCP(connection *connListItem, char *buffer);

int recvMessageUDP(int FD, char *buffer, struct sockaddr *clientAddr, socklen_t clientAddrSize);

int reckognizeService(connection *connListItem);

int sendMessage(int sockFD, connection *connListItem, char *buffer);

int checkIPset(connection *connListItem, int srvNo);

int responseFromService(int sockFD, connection *connListItem, char *buffer, int numOfService);

void cleanConnFields(connection *connListItem);

int dataExchangeWithClient(int serverSock, connection *connList, struct epoll_event *evListItem);

char* getDestinationIP(int socketFD);

void sig_handler(int signum);

#endif /* SERVFUNCTIONS_H_ */

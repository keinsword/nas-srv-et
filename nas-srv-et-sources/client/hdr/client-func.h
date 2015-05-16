/*
 * clientFunctions.h
 *
 *  Created on: Apr 11, 2015
 *      Author: keinsword
 */

#ifndef CLIENTFUNCTIONS_H_
#define CLIENTFUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <math.h>
#include <fcntl.h>
#include "../../../nas-srv-et-lib/include/nas-srv-et-lib.h"

extern config clientConfig;

extern struct sockaddr_in serverAddr;
extern socklen_t serverAddrSize;

extern const char *exitpr;

void errTableInit();

int checkArgs(char *args[], connection *conn);

int createClientSocket();

int setMessageText(connection *conn);

int sendMessageToServer(int sockFD, connection *conn);

int recvMessageFromServer(int sockFD, connection *conn);

#endif /* CLIENTFUNCTIONS_H_ */

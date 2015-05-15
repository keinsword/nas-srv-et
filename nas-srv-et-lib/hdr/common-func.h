/*
 * commonFunctions.h
 *
 *  Created on: Apr 16, 2015
 *      Author: keinsword
 */

#ifndef COMMONFUNCTIONS_H_
#define COMMONFUNCTIONS_H_

#include <fcntl.h>

typedef enum {
	TCP, UDP
} transport;

typedef struct {
	const char *address;
	unsigned short port;
	const char *transport;
	int qlen;
} config;

extern transport transp_proto;

int fdSetBlocking(int fd, int blocking);

#endif /* COMMONFUNCTIONS_H_ */

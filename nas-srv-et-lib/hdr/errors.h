/*
 * errors.h
 *
 *  Created on: Apr 19, 2015
 *      Author: keinsword
 */

#ifndef ERRORS_H_
#define ERRORS_H_

//codes of the common errors
#define INCORRECT_ARGS -1
#define SOCKET_ERR -2
#define READ_ERR -3
#define CHECKSUM_ERR -4
#define SEND_ERR -5

//codes of the server errors
#define FD_BLOCKING_ERR -6
#define SET_SOCK_OPT_ERR -7
#define BIND_ERR -8
#define LISTEN_ERR -9
#define ACCEPT_ERR -10
#define NOT_ENOUGH_SPACE -11
#define IDENTIFY_ERR -12
#define SERVICE_NAME_ERR -13
#define SERVICE_IP_ERR -22
#define PART_SEND_ERR -14
#define CONFIG_READ_ERR -15
#define PART_READ_ERR -16
#define INVALID_SIGNATURE -17

//codes of the client errors
#define WRONG_SRV_REQ -18
#define NO_MORE_PLACE -19
#define SRV_IS_OFFLINE -20
#define CONNECT_ERR -21
#define WRONG_SRV_IP_REQ -23

//codes of the non-error return values
#define READ_AGAIN 1
#define CLIENT_IS_OFFLINE 2
#define EXITPR 3

typedef struct {
	short errCode;
	char *errDesc;
	short isCritical;
} error;

extern error errTable[];

void handleErr(short errCode);

#endif /* ERRORS_H_ */

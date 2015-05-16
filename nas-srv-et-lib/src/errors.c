#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include "../hdr/errors.h"

error errTable[] = {
		//codes of the common errors
		{ INCORRECT_ARGS, "Incorrect program arguments!\n", 1 },
		{ SOCKET_ERR, "Error creating the socket!\n", 1 },
		{ READ_ERR, "Error reading new message!\n", 0 },
		{ CHECKSUM_ERR, "Checksum missmatch!\n", 0 },
		{ SEND_ERR, "Error sending message!\n", 0 },

		//codes of the server errors
		{ FD_BLOCKING_ERR, "Error switching socket FD to blocking mode!\n", 1 },
		{ SET_SOCK_OPT_ERR, "Error setting option to socket FD!\n", 1 },
		{ BIND_ERR, "Can't bind!\n", 1 },
		{ LISTEN_ERR, "Error switching socket to the listening state!\n", 1 },
		{ ACCEPT_ERR, "Can't accept the connection with client!\n", 0 },
		{ NOT_ENOUGH_SPACE, "The new client tried to connect, but there's no more place!\n", 0 },
		{ IDENTIFY_ERR, "Can't identify the client!\n", 0 },
		{ SERVICE_NAME_ERR, "Non-existing service was requested by the client!\n", 0 },
		{ SERVICE_IP_ERR, "IP-address is incorrect for the requested service name!\n", 0 },
		{ PART_SEND_ERR, "Error sending message by parts!\n", 0 },
		{ CONFIG_READ_ERR, "Reading config is crashed!\n", 1 },
		{ PART_READ_ERR, "Error reading message by parts!\n", 0 },
		{ INVALID_SIGNATURE, "Invalid protocol signature!\n", 0 },

		//codes of the client errors
		{ WRONG_SRV_REQ, "You have requested a non-existing service!", 1 },
		{ WRONG_SRV_IP_REQ, "Requested service is unavailable for you!", 1 },
		{ NO_MORE_PLACE, "Can't connect to server: no more place!", 1 },
		{ SRV_IS_OFFLINE, "Server is offline!", 1 },
		{ CONNECT_ERR, "Can't connect to server!", 1 }
};

void handleErr(short errCode) {
	int i, n;
	for(i = 0; i < sizeof(errTable)/sizeof(errTable[0]); i++)
		if(errTable[i].errCode == errCode) {
			n = i;
			break;
		}
	fprintf(stderr, "%s", errTable[n].errDesc);
	syslog(LOG_INFO, "%s", errTable[n].errDesc);
	if(errTable[n].isCritical == 1)
		exit(EXIT_FAILURE);
}

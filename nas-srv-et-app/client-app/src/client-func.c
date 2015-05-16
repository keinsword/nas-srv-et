#include "../hdr/client-func.h"

const char *exitpr = "exitpr";

int checkArgs(char *args[], connection *conn) {
	int port = atoi(args[2]);
	const char *transport = args[3];
	const char *nickname = args[4];
	const char *service_name = args[5];

	if((port < 1024) || (port > 65535) || (strncmp(transport, "udp", strlen(transport)) && strncmp(transport, "tcp", strlen(transport))))
		return INCORRECT_ARGS;
	else if((strlen(nickname) < 4) && (strlen(nickname) > 15))
		return INCORRECT_ARGS;
	else if(strlen(service_name) > 8)
		return INCORRECT_ARGS;
	else {
		clientConfig.address = args[1];
		clientConfig.port = atoi(args[2]);

		if(!strncmp(args[3], "udp", strlen(args[3])))
			transp_proto = UDP;
		else
			transp_proto = TCP;

		sprintf(conn->clientNickName, "%s", nickname);
		sprintf(conn->serviceName, "%s", service_name);

		return 0;
	}
}

int createClientSocket() {
	int sockFD;
	int type, proto;

	switch(transp_proto) {
	case UDP:
		type = SOCK_DGRAM;
		proto = IPPROTO_UDP;
		break;
	case TCP:
		type = SOCK_STREAM;
		proto = IPPROTO_TCP;
		break;
	}

	sockFD = socket(PF_INET, type, proto);
	if (sockFD < 0)
		return SOCKET_ERR;

	serverAddr.sin_port = htons(clientConfig.port);
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, clientConfig.address, &serverAddr.sin_addr);

	serverAddrSize = sizeof(serverAddr);

	if(type == SOCK_STREAM) {
		if(connect(sockFD, (struct sockaddr *) &serverAddr, serverAddrSize) < 0)
			return CONNECT_ERR;
		printf("Successfully connected to %s:%d!\n\n", clientConfig.address, clientConfig.port);
	}
	else
		printf("Client was started. Using UDP protocol.\n\n");
	return sockFD;
}

int setMessageText(connection *conn) {
	char temp[MSGSIZE];
	memset(&temp, 0, sizeof(temp));

	printf("Type the message text: ");
	fgets(temp, sizeof(temp), stdin);
	strncpy(conn->messageText, temp, strlen(temp)-1);
	if (strcmp(conn->messageText, exitpr) == 0)
		return EXITPR;
	return 0;
}

int sendMessageToServer(int sockFD, connection *conn) {
	int result;
	size_t msgSize;
	char buffer[BUFFERSIZE];
	//char buffer1[BUFFERSIZE]; int n;

	memset(&buffer, 0, sizeof(buffer));
	//memset(&buffer1, 0, sizeof(buffer1));

	Serializer(conn, buffer);

/*	strncpy(buffer1, buffer, 25);
	n = write(sockFD, buffer1, strlen(buffer1));
	memset(&buffer1, 0, sizeof(buffer1));
	int i, j;
	for(i = 25, j = 0; i < 30; i++, j++)
		buffer1[j]=buffer[i];
	sleep(5);
	n = write(sockFD, buffer1, strlen(buffer1));
	memset(&buffer1, 0, sizeof(buffer1));
	for(i = 30, j = 0; i < strlen(buffer); i++, j++)
		buffer1[j]=buffer[i];
	sleep(5);
	n = write(sockFD, buffer1, strlen(buffer1));
	memset(&buffer1, 0, sizeof(buffer1));*/

	msgSize = strlen(buffer);

	switch(transp_proto) {
	case UDP:
		if(msgSize > MTU) {
			if(Divider(sockFD, buffer) < 0)
				return PART_SEND_ERR;
		}
		else
			result = sendto(sockFD, buffer, strlen(buffer), 0, (struct sockaddr *) &serverAddr, serverAddrSize);
		break;
	case TCP:
		result = write(sockFD, buffer, strlen(buffer));
		break;
	}
	if (result == -1)
		return SEND_ERR;
	else {
		printf("Sent message: %s\n", conn->messageText);
		return result;
	}
}

int recvMessageFromServer(int sockFD, connection *conn) {
	int result;
	char buffer[BUFFERSIZE];
	memset(&buffer, 0, sizeof(buffer));

	switch(transp_proto) {
	case TCP:
		result = read(sockFD, buffer, sizeof(buffer));
		break;
	case UDP:
		result = recvfrom(sockFD, buffer, sizeof(buffer), 0, (struct sockaddr *) &serverAddr, &serverAddrSize);
		break;
	}
	if (result == -1)
		return READ_ERR;
	else {
		if(strncmp(buffer, NO_MORE_PLACE_NOTIF, strlen(NO_MORE_PLACE_NOTIF)) == 0)
			return NO_MORE_PLACE;

		if(strcmp(WRONG_SRV_NOTIF, buffer) == 0)
			return WRONG_SRV_REQ;

		if(strcmp(SRV_IS_OFFLINE_NOTIF, buffer) == 0)
			return SRV_IS_OFFLINE;

		if(strcmp(WRONG_SRV_IP_NOTIF, buffer) == 0)
			return WRONG_SRV_IP_REQ;

		deSerializer(conn, buffer);
		printf("Server response: %s\n\n", conn->messageText);
		memset(&conn->messageText, 0, sizeof(conn->messageText));
		return result;
	}
}

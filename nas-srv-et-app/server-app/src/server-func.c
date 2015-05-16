#include "../hdr/server-func.h"

int errno;

struct sockaddr_in clientAddr;
socklen_t clientAddrSize = sizeof(clientAddr);

int readConfigFile(config_t *cfg) {
	int i;
	config_setting_t *setting;
	char IPsetPath[100];
	const char *defaultServicesPath = "application.services";

	if (!config_read_file(cfg, "server_config.cfg")) {
		config_destroy(cfg);
		return CONFIG_READ_ERR;
	}

	setting = config_lookup(cfg, "application.services.serviceNamesList");
	if(setting!= NULL) {
		num_of_services = config_setting_length(setting);

		for (i = 0; i < num_of_services; i++)
			srvInfoTable[i].srv_name = config_setting_get_string_elem(setting, i);
	}

	for (i = 0; i < num_of_services; i++) {
		sprintf(IPsetPath, "%s.%s.%s", defaultServicesPath, srvInfoTable[i].srv_name, "service_Ip");

		setting = config_lookup(cfg, IPsetPath);
		if(setting != NULL){
			srvInfoTable[i].ip_in_set = config_setting_length(setting);
			int j;
			for (j = 0; j < srvInfoTable[i].ip_in_set; j++)
				srvInfoTable[i].ip_set[j] = config_setting_get_string_elem(setting, j);
		}
	}

	//Поиск строчки port
	if(config_lookup_int(cfg, "application.connectSocket.port", &servConfig.port))
		//Поиск строчки transport
		if(config_lookup_string(cfg, "application.connectSocket.transport", &servConfig.transport))
			//Поиск строчки qlen
			if(config_lookup_int(cfg, "application.connectSocket.qlen", &servConfig.qlen))
				return 0;

	return CONFIG_READ_ERR;
}

int checkArgs() {
	if((servConfig.port < 1024) || (servConfig.port > 65535) || (strcmp(servConfig.transport, "udp") && strcmp(servConfig.transport, "tcp")))
		return INCORRECT_ARGS;
	else {
		if(!strcmp(servConfig.transport, "udp"))
			transp_proto = UDP;
		else
			transp_proto = TCP;
		return 0;
	}
}

void timeoutCheck(connection *connList) {
	time_t timeout;
	int i = 0;

	for(i = 0; i < NUM_OF_CONNECTIONS; i++) {
		timeout = time(NULL) - connList[i].timeout;
		if((timeout > TIMEOUT) && (connList[i].clientHostName[0] != '\0')) {
			syslog(LOG_INFO, "Timeout period for \"%s\" has experied.\n", connList[i].clientHostName);
			close(connList[i].clientSockFD);
			memset(&connList[i], 0, sizeof(connList[i]));
			break;
		}
	}
}

int createServerSocket() {
	struct sockaddr_in sin;
	int s, type, proto, optval = 1;

	memset(&sin, 0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(servConfig.port);

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

	s = socket(PF_INET, type, proto);
	if (s < 0)
		return SOCKET_ERR;

	fdSetBlocking(s, 0);
	if (s < 0)
		return FD_BLOCKING_ERR;

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		return SET_SOCK_OPT_ERR;

	if(bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		return BIND_ERR;

	if(type == SOCK_STREAM) {
		if(listen(s, servConfig.qlen) < 0)
			return LISTEN_ERR;
	}
	return s;
}

int acceptNewConnection(int listeningSocket, connection *connList, struct epoll_event *evListItem) {
	int clientSocket, i;

	clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

	printf("Stored FD: %d\n", clientSocket);

	if (clientSocket < 0) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			return 0;
		else
			return ACCEPT_ERR;
	}

	for(i = 0; i < NUM_OF_CONNECTIONS; i++) {
		if(connList[i].clientHostName[0] == '\0') {
			fdSetBlocking(clientSocket, 0);
			connList[i].clientSockFD = clientSocket;
			connList[i].netns_sock_fd = evListItem->data.fd;
			connList[i].timeout = time(NULL);
			getnameinfo((struct sockaddr *)&clientAddr, clientAddrSize, connList[i].clientHostName, sizeof(connList[i].clientHostName), NULL, 0, 0);
			sprintf(connList[i].destinationIpAddr, "%s", getDestinationIP(clientSocket));
			printf("Dest ip: %s\n", connList[i].destinationIpAddr);
			event.data.fd = clientSocket;
			event.events = EPOLLIN;
			epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocket, &event);

			syslog(LOG_INFO, "Accepted connection on descriptor %d (hostname: %s)!\n", clientSocket, connList[i].clientHostName);

			return clientSocket;
		}
	}
	if(write(clientSocket, NO_MORE_PLACE_NOTIF, strlen(NO_MORE_PLACE_NOTIF)) < 0)
		return SEND_ERR;

	return NO_MORE_PLACE;
}

int identifySenderTCP(connection *connList, struct epoll_event *evListItem) {
	int i;
	for (i = 0; i < NUM_OF_CONNECTIONS; i++) {
		printf("Compare FDs: %d | %d\n", evListItem->data.fd, connList[i].clientSockFD);
		if (evListItem->data.fd == connList[i].clientSockFD)
			return i;
	}
	return IDENTIFY_ERR;
}

int identifySenderUDP(connection *connList, char *buffer) {
	int i;
	connection tempConn;
	memset(&tempConn, 0, sizeof(tempConn));

	deSerializer(&tempConn, buffer);

	for(i = 0; i < NUM_OF_CONNECTIONS; i++) {
		if((strcmp(connList[i].clientNickName, tempConn.clientNickName) || (connList[i].clientNickName[0] == '\0')) == 0)
			return i;
	}
	return IDENTIFY_ERR;
}

int serverChecksumCalculateAndCompare(connection *connListItem, char *buffer) {
	char crcMessage[BUFFERSIZE];
	char crcServerResult[CRC32SIZE];
	memset(&crcMessage, 0, sizeof(crcMessage));
	memset(&crcServerResult, 0, sizeof(crcServerResult));

	strncpy(crcMessage, buffer, strlen(buffer) - 8);
	sprintf(crcServerResult, "%X", (unsigned int)crcSlow((unsigned char *)crcMessage, strlen(crcMessage)));

	if (strcmp(connListItem->messageCRC32, crcServerResult) == 0) {
		memset(&buffer, 0, BUFFERSIZE);
		return 0;
	}
	else
		return CHECKSUM_ERR;
}

int recvMessageFromClient(int FD, connection *connListItem, char *buffer) {
	int result;

	if(transp_proto == TCP)
		result = read(connListItem->clientSockFD, buffer, BUFFERSIZE);
	else
		result = recvfrom(FD, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrSize);
	if (result == -1) {
		if(errno != EAGAIN)
			return READ_ERR;
		else
			return 0;
	}
	printf("%s\n", buffer);
	if (transp_proto == TCP) {
		if(result == 0) {
			syslog(LOG_INFO, "Client \"%s\" (at %s) has closed the connection.\n", connListItem->clientNickName, connListItem->clientHostName);
			printf("ssss\n");
			return CLIENT_IS_OFFLINE;
		}
	}
	else {
		clientAddrSize = sizeof(clientAddr);

		if(strncmp(buffer, SEG_WARNING, strlen(SEG_WARNING)) == 0)
			if(Assembler(FD, buffer, &clientAddr, clientAddrSize) < 0)
				return PART_READ_ERR;
	}

	return result;
}

int reckognizeService(connection *connListItem) {
	int i;
	for(i = 0; i < num_of_services; i++) {
		printf("Comp srv names: %s | %s\n", connListItem->serviceName, srvInfoTable[i].srv_name);
		if(strncmp(connListItem->serviceName, srvInfoTable[i].srv_name, strlen(srvInfoTable[i].srv_name)) == 0)
			if(connListItem->netns_sock_fd == srvInfoTable[i].netns_fd)
				return i;
			else {
			        if(write(connListItem->clientSockFD, WRONG_SRV_IP_NOTIF, strlen(WRONG_SRV_IP_NOTIF)) < 0)
			                return SEND_ERR;
				close(connListItem->clientSockFD);
				memset(&connListItem, 0, sizeof(connListItem));
				return SERVICE_IP_ERR;
			}
	}

	if(write(connListItem->clientSockFD, WRONG_SRV_NOTIF, strlen(WRONG_SRV_NOTIF)) < 0)
		return SEND_ERR;

	close(connListItem->clientSockFD);
	memset(&connListItem, 0, sizeof(connListItem));

	return SERVICE_NAME_ERR;
}

int checkIPset(connection *connListItem, int srvNo) {
	int i;
	for(i = 0; i < srvInfoTable[srvNo].ip_in_set; i++) {
		printf("Comp IPs: %s | %s\n", connListItem->destinationIpAddr, srvInfoTable[srvNo].ip_set[i]);
		if(strncmp(connListItem->destinationIpAddr, srvInfoTable[srvNo].ip_set[i], strlen(srvInfoTable[srvNo].ip_set[i])) == 0)
			return 1;
	}
	if(write(connListItem->clientSockFD, WRONG_SRV_IP_NOTIF, strlen(WRONG_SRV_IP_NOTIF)) < 0)
		return SEND_ERR;

	close(connListItem->clientSockFD);
	memset(&connListItem, 0, sizeof(connListItem));

	return SERVICE_IP_ERR;
}

int responseFromService(int sockFD, connection *connListItem, char *buffer, int numOfService) {
	int result;

	strcat(connListItem->messageText, srvResponseTemplate);
	strcat(connListItem->messageText, srvInfoTable[numOfService].srv_name);

	Serializer(connListItem, buffer);

	switch(transp_proto) {
	case TCP:
		result = write(connListItem->clientSockFD, buffer, strlen(buffer));
		break;
	case UDP:
		result = sendto(sockFD, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, clientAddrSize);
		break;
	}

	if(result < 0)
		return SEND_ERR;
	else
		return result;
}

int sendMessage(int sockFD, connection *connListItem, char *buffer) {
	int srv;
	srv = reckognizeService(connListItem);
	if(srv < 0)
		return srv;
	if(checkIPset(connListItem, srv) == 1) {
		syslog(LOG_INFO, "Client \"%s\" has received service \"%s\".", connListItem->clientNickName, srvInfoTable[srv].srv_name);
		return responseFromService(sockFD, connListItem, buffer, srv);
	}
	else
		return SERVICE_IP_ERR;
}

void cleanConnFields(connection *connListItem) {
	memset(&connListItem->messageText, 0, sizeof(connListItem->messageText));
	memset(&connListItem->messageCRC32, 0, sizeof(connListItem->messageCRC32));
	memset(&connListItem->length, 0, sizeof(connListItem->length));
	memset(&connListItem->storageBuffer, 0, sizeof(connListItem->storageBuffer));
}

int dataExchangeWithClient(int serverSock, connection *connList, struct epoll_event *evListItem) {
	int n, result;
	char buffer[BUFFERSIZE];
	memset(&buffer, 0, sizeof(buffer));

	if(transp_proto == TCP)
		n = identifySenderTCP(connList, evListItem);
	else
		n = identifySenderUDP(connList, buffer);
	if(n < 0)
		return n;

	result = recvMessageFromClient(serverSock, &connList[n], buffer);
	if((result < 0))
		return result;
	if(result == CLIENT_IS_OFFLINE) {
		close(connList[n].clientSockFD);
		memset(&connList[n].serviceName, 0, sizeof(connList[n].serviceName));
		memset(&connList[n].clientNickName, 0, sizeof(connList[n].clientNickName));
		memset(&connList[n].messageText, 0, sizeof(connList[n].messageText));
		memset(&connList[n].clientHostName, 0, sizeof(connList[n].clientHostName));
		connList[n].clientSockFD = 0;
		return CLIENT_IS_OFFLINE;
	}

	deSerializer(&connList[n], buffer);

	result = serverChecksumCalculateAndCompare(&connList[n], buffer);
	if (result == 0) {
		connList[n].timeout = time(NULL);
		result = sendMessage(serverSock, &connList[n], buffer);
		if(result < 0)
			return result;
	}
	else {
		if(transp_proto == TCP)
			write(connList[n].clientSockFD, CRC_MISSMATCH_NOTIF, strlen(CRC_MISSMATCH_NOTIF));
		else
			sendto(serverSock, CRC_MISSMATCH_NOTIF, strlen(CRC_MISSMATCH_NOTIF), 0, (struct sockaddr *)&clientAddr, clientAddrSize);
		return result;
	}

	cleanConnFields(&connList[n]);

	return 0;
}

char* getDestinationIP(int socketFD) {
	char *destIP = NULL;
    struct in_addr inAddr;
	struct sockaddr_in sockAddr_in;
    socklen_t sockAddr_in_len = sizeof(sockAddr_in);

    memset(&inAddr, 0, sizeof(inAddr));
    memset(&sockAddr_in, 0, sizeof(sockAddr_in));

    if (getsockname(socketFD, &sockAddr_in, &sockAddr_in_len) == -1)
        return 0;

    inAddr.s_addr = sockAddr_in.sin_addr.s_addr;

    destIP = inet_ntoa(inAddr);

    return destIP;
}

void sig_handler(int signum) {
	switch (signum) {
	case SIGINT:
		syslog(LOG_INFO, "Daemon is off.");
		endEventLoop = 1;
		endMainLoop = 1;
		return;
	case SIGHUP:
		syslog(LOG_INFO, "Restarting daemon. Reloading config...");
		endEventLoop = 1;
		return;
	}
}

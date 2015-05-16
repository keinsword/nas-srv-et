#include "../hdr/protocol.h"

const char SEG_WARNING[] = "SEGMENTATED_MESSAGES_WILL_COME:";
const char ACK[] = "ACK\0";

const char *srvResponseTemplate = "  //Response from service ";

void Serializer(connection *conn, char *buffer) {
	char header[MAX_HEADER_SIZE];
	char payload[MAX_PAYLOAD_SIZE];
	char header_payload[MAX_HEADER_SIZE + MAX_PAYLOAD_SIZE];
	char tailer[CRC32SIZE];

	memset(&header, 0, sizeof(header));
	memset(&payload, 0, sizeof(payload));
	memset(&header_payload, 0, sizeof(header_payload));
	memset(&tailer, 0, sizeof(tailer));

	sprintf(payload, "%s|%s|%s|", conn->clientNickName, conn->serviceName, conn->messageText);
	sprintf(header, "%s|%s|%d|", conn->protoName, conn->protoVersion, (int)strlen(payload) + 8);
	sprintf(header_payload, "%s%s", header, payload);
	sprintf(tailer, "%X", (unsigned int)crcSlow((unsigned char *)header_payload, strlen(header_payload)));
	sprintf(buffer, "%s%s%s", header, payload, tailer);
}

void deSerializer(connection *connListItem, char *buffer) {
	int i = 0, j = 0;
	char temp[5];
	memset(&temp, 0, sizeof(temp));

	while(buffer[i] != 124) {
		connListItem->protoName[j] = buffer[i];
		i++;
		j++;
	}
	i++;
	j = 0;
	while(buffer[i] != 124) {
		connListItem->protoVersion[j] = buffer[i];
		i++;
		j++;
	}
	i++;
	j = 0;
	while(buffer[i] != 124) {
		temp[j] = buffer[i];
		i++;
		j++;
	}
	connListItem->length = atoi(temp);
	i++;
	j = 0;
	while(buffer[i] != 124) {
		connListItem->clientNickName[j] = buffer[i];
		i++;
		j++;
	}
	i++;
	j = 0;
	while(buffer[i] != 124) {
		connListItem->serviceName[j] = buffer[i];
		i++;
		j++;
	}
	i++;
	j = 0;
	while(buffer[i] != 124) {
		connListItem->messageText[j] = buffer[i];
		i++;
		j++;
	}
	i++;
	j = 0;
	while(buffer[i] != 0) {
		connListItem->messageCRC32[j] = buffer[i];
		i++;
		j++;
	}
	printf("de serializer is ok\n");
}

int Divider(int sockFD, char *buffer) {
	int segNum, i, j, k;

	char segWarning[25];
	char tempBuffer[5];

	char segArray[NUM_OF_SEGMENTS][MTU+1];

	memset(&segWarning, 0, sizeof(segWarning));
	memset(&tempBuffer, 0, sizeof(tempBuffer));
	memset(&segArray, 0, sizeof(segArray));

	if((strlen(buffer) % MTU) == 0)
		segNum = strlen(buffer) / MTU;
	else
		segNum = strlen(buffer) / MTU + 1;

	sprintf(segWarning, "%s%d", SEG_WARNING, segNum);

	fdSetBlocking(sockFD, 1);

	if(sendto(sockFD, segWarning, strlen(segWarning), 0, (struct sockaddr *) &serverAddr, serverAddrSize) < 0)
		return SEND_ERR;

	if((recvfrom(sockFD, tempBuffer, sizeof(tempBuffer), 0, (struct sockaddr *) &serverAddr, &serverAddrSize) > 0) && (strcmp(tempBuffer, ACK) == 0)) {
		memset(&tempBuffer, 0, sizeof(tempBuffer));

		for(j = 0; j < segNum; j++)
			for(i = (j*strlen(buffer))/segNum, k = 0; i < ((j+1)*strlen(buffer))/segNum; i++, k++)
				segArray[j][k] = buffer[i];

		for(j = 0; j < segNum; j++) {
			memset(&tempBuffer, 0, sizeof(tempBuffer));

			if(sendto(sockFD, segArray[j], strlen(segArray[j]), 0, (struct sockaddr *) &serverAddr, serverAddrSize) < 0)
				return SEND_ERR;

			if((recvfrom(sockFD, tempBuffer, sizeof(tempBuffer), 0, (struct sockaddr *) &serverAddr, &serverAddrSize) < 0) || !strcmp(tempBuffer, ACK))
				return READ_ERR;
		}
	}
	else
		return READ_ERR;

	fdSetBlocking(sockFD, 0);

	return segNum;
}

int Assembler(int sockFD, char *buffer, struct sockaddr *clientAddr, socklen_t clientAddrSize) {
	int i, j, segNum;

	char strSegNum[5];
	char accumBuffer[BUFFERSIZE];
	char tempBuffer[MTU+1];

	memset(&strSegNum, 0, sizeof(strSegNum));
	memset(&accumBuffer, 0, sizeof(accumBuffer));
	memset(&tempBuffer, 0, sizeof(tempBuffer));
	memset(buffer, 0, BUFFERSIZE);

	for(i = 0, j = strlen(SEG_WARNING); j < strlen(buffer); i++, j++)
		strSegNum[i] = buffer[j];
	segNum = atoi(strSegNum);

	fdSetBlocking(sockFD, 1);

	if(sendto(sockFD, ACK, strlen(ACK), 0, clientAddr, clientAddrSize) < 0)
		return SEND_ERR;

	for(i = 0; i < segNum; i++) {
		if(recvfrom(sockFD, tempBuffer, sizeof(tempBuffer), 0, clientAddr, &clientAddrSize) > 0) {
			strncat(buffer, tempBuffer, strlen(tempBuffer));
			memset(&tempBuffer, 0, sizeof(tempBuffer));
			if(sendto(sockFD, ACK, strlen(ACK), 0, clientAddr, clientAddrSize) < 0)
				return SEND_ERR;
		}
		else
			return READ_ERR;
	}

	fdSetBlocking(sockFD, 0);

	return segNum;
}

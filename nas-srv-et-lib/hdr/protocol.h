#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define MTU 1500			//максимальный размер передаваемых данных
#define TIMEOUT 900		//тайм-аут в секундах

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common-func.h"
#include "crc.h"
#include "errors.h"

#define PROTO_NAME "NAS_SRV_EMULATOR"	//имя протокола
#define PROTO_VER "0.1"					//версия прокола

#define NUM_OF_CONNECTIONS 100	//число активных соединений
#define NUM_OF_SEGMENTS 100		//максимальное число сегментов

#define BUFFERSIZE 2048			//длина буфера
#define MSGSIZE 1536			//длина сообщения
#define CRC32SIZE 10			//длина контрольной суммы
#define NICK_SIZE 17			//длина никнейма
#define SERVICE_SIZE 10			//длина имени сервиса

#define MAX_HEADER_SIZE 30
#define MAX_PAYLOAD_SIZE 1560

#define WRONG_SRV_NOTIF "You have requested a non-existent service"
#define WRONG_SRV_IP_NOTIF "You have requested service by incorrect IP-address"
#define NO_MORE_PLACE_NOTIF "No more place for new clients"
#define CRC_MISSMATCH_NOTIF "Checksum missmatch"
#define SRV_IS_OFFLINE_NOTIF "Server is offline now"

extern const char SEG_WARNING[];
extern const char ACK[];

extern const char *srvResponseTemplate;

extern struct sockaddr_in serverAddr;
extern socklen_t serverAddrSize;

//структура, описывающая соединение и его параметры
typedef struct {
	char protoName[20];					//имя протокола
	char protoVersion[5];				//версия протокола
	short length;						//длина сообщения
	int clientSockFD;					//файловый дескриптор клиентского сокета
	char clientHostName[17];			//хостнейм клиента
	char clientNickName[NICK_SIZE];		//ник пользователя
	char serviceName[SERVICE_SIZE];		//имя сервиса
	char messageText[MSGSIZE];			//текст сообщения
	char messageCRC32[CRC32SIZE];		//контрольная сумма
	int timeout;						//тайм-аут
	char destinationIpAddr[17];
	char storageBuffer[BUFFERSIZE];
	short segmentationFlag;
	short lengthRecv;
} connection;

void Serializer(connection *connection, char *buffer);

void deSerializer(connection *connListItem, char *buffer);

int Divider(int sockFD, char *buffer);

int Assembler(int sockFD, char *buffer, struct sockaddr *clientAddr, socklen_t clientAddrSize);

#endif /* PROTOCOL_H_ */

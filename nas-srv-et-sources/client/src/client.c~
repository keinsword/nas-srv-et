#include "../hdr/client-func.h"

config clientConfig;
transport transp_proto;
struct sockaddr_in serverAddr;
socklen_t serverAddrSize;
static int result;

int dataExchangeWithServer(connection *conn, int sockFD);

int main(int argc, char *argv[]) {
	int sockFD;
	connection conn;

	memset(&conn, 0, sizeof(conn));

	strcpy(conn.protoName, PROTO_NAME);
	strcpy(conn.protoVersion, PROTO_VER);

	errTableInit();

    if (argc == 4) {
  		result = checkArgs(argv);
		if(result < 0)
		 	handleErr(result);

    	sockFD = createClientSocket();
    	if(sockFD < 0)
    		handleErr(sockFD);

    	getClientInfo(&conn);

    	while(1) {
    		result = dataExchangeWithServer(&conn, sockFD);
    		if(result < 0)
    			handleErr(result);
    	}

    	close(sockFD);
    }
    else
    	printf("Usage: %s address port transport\n", argv[0]);

    return 0;
}

int dataExchangeWithServer(connection *conn, int sockFD) {
	result = setMessageText(conn);
	if(result < 0)
		return result;

	result = sendMessageToServer(sockFD, conn);
	if(result < 0)
		return result;

	result = recvMessageFromServer(sockFD, conn);
	if(result < 0)
		return result;

	return 0;
}

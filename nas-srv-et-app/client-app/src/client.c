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

    if (argc == 6) {
  		result = checkArgs(argv, &conn);
		if(result < 0)
		 	handleErr(result);

    	sockFD = createClientSocket();
    	if(sockFD < 0)
    		handleErr(sockFD);

    	while(1) {
    		result = dataExchangeWithServer(&conn, sockFD);
    		if(result < 0)
    			handleErr(result);
    	}

    	close(sockFD);
    }
    else
    	printf("Usage: %s \"address\" \"port\" \"transport\" \"nickname\" \"service_name\"\n
		Length of the nickname must be from 4 to 15 symbols.\n
		Length of the service_name must be less than 8 symbols.\n", argv[0]);

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

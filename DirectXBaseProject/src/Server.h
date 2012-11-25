#ifndef SERVER_H
#define SERVER_H

#include "NetworkDefines.h"

class Server{
public:
	Server(void);
	~Server(void);

	bool			StartServer();

private:
	bool			StartWinSock();

private:
	sockaddr_in		me;
	sockaddr		you;
	int				numConnections;			// Keep track of number of connections
	SOCKET			connSocket;
};

#endif
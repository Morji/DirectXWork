#include "Server.h"

Server::Server(){
	
}

Server::~Server(){
	closesocket(connSocket);
	WSACleanup();
}

bool Server::StartServer(){
	if (!StartWinSock())
		return false;

	std::cout << "WinSock Started \n";
	connSocket = socket(AF_INET,SOCK_DGRAM,0); // create socket

	me.sin_family = AF_INET;
	me.sin_port = htons (SERVERPORT);
	me.sin_addr.s_addr = htonl (INADDR_ANY);

	printf("The IP address being connected to is: ");

	return true;
}

bool Server::StartWinSock(){
	// Startup Winsock
	WSADATA w;
	int error = WSAStartup (0x0202,&w);
	if (error){
		printf("Error:  You need WinSock 2.2!\n");
		return false;
	}
	if (w.wVersion!=0x0202){
		printf("Error:  Wrong WinSock version!\n");
		WSACleanup ();
		return false;
	}

	return true;
}
#ifndef _HSFSOCKET_H_
#define _HSFSOCKET_H_

#define USEWINSOCK
//#define USEUNIX

#ifdef USEUNIX
	#include <netinet/in.h>		//IPv4 socket address structures
#endif

#ifdef USEWINSOCK
	#include <winsock2.h>		// This must be before windows.h
	#include <windows.h>		// to prevent incluseion of winsock.h
#endif

#ifdef USEWINSOCK
	typedef  int socklen_t;
#endif

#ifdef USEUNIX
	typedef  int SOCKET;
#endif

#include "NetworkDefines.h"

class CUDPSocket
{
public:
	CUDPSocket();
	~CUDPSocket();
	
	int MakeNonBlocking(void);
	int Initialise(void);
	int Bind(const int Port);
	int Receive(char * Buffer);
	int ReceiveFrom(char *Buffer, sockaddr_in& address);
	int Send(char * Buffer);
	int SendTo(char * Buffer, sockaddr_in& address);
	void SetDestinationAddress(char * IP, const int Port);
	sockaddr_in GetDestinationAddress(void);
	sockaddr_in GetLocalAddress(void);

	SOCKET GetSocket();
protected:
	SOCKET m_Socket;
	socklen_t m_SocketAddressSize;
	int m_Ret;
	struct sockaddr_in m_LocalAddress, m_RemoteAddress;

#ifdef USEWINSOCK
	WSADATA m_WSData;
#endif
};

#endif
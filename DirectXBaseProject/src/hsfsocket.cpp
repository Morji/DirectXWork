#include "hsfsocket.h"

#ifdef USEUNIX
	#include <stdio.h>
	#include <unistd.h>			//UNIX stuff needed for close()
	#include <netinet/in.h>		//IPv4 socket address structures
	#include <netdb.h>			//Access to DNS lookup
	#include <arpa/inet.h>		//inet_ntop functions
	#include <sys/socket.h>		//Socket functions
	#include <fcntl.h>
#endif

#ifdef USEWINSOCK
	#include <stdio.h>
#endif

#ifdef USEWINSOCK
	#pragma comment (lib, "ws2_32.lib")
#endif

CUDPSocket::CUDPSocket()
{
	m_SocketAddressSize = sizeof(sockaddr_in);
	m_Socket = 0;
}

CUDPSocket::~CUDPSocket()
{
#ifdef USEUNIX
	if(m_Socket)close(m_Socket);
#endif
	
#ifdef USEWINSOCK
	if(m_Socket)closesocket(m_Socket);
	WSACleanup ();
#endif
	
	m_Socket = 0;
}


int CUDPSocket::MakeNonBlocking(void)
{
	
#ifdef USEUNIX
	if ((fcntl(m_Socket, F_SETFL, O_NONBLOCK)) < 0)
		{
			printf("Error:  Can't make Socket nonblocking\n");
			return 0;
		}
#endif
		
#ifdef USEWINSOCK
	u_long NonBlocking = TRUE;
	if ((ioctlsocket(m_Socket, FIONBIO, (u_long*)&NonBlocking)) == SOCKET_ERROR)
		{
			printf("Error:  Can't make Socket nonblocking\n");
			return 0;
		}
#endif	
		
	return 1;
}
	
int CUDPSocket::Initialise(void)
{
#ifdef USEWINSOCK
	int error = WSAStartup (0x0202,&m_WSData);
	if (error)
	{
		printf("You need WinSock 2.2\n");
		return 0;
	}
	if (m_WSData.wVersion!=0x0202)
	{
		printf("Error:  Wrong WinSock version!\n");
		WSACleanup ();
		return 0;
	}
#endif
	
	// socket creation
	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_Socket<0) 
	{
		printf("Cannot open socket \n");
		return 0;
	}
	
	return 1;
}

int CUDPSocket::Bind(const int Port)
{
	/* bind socket to local port */
	m_LocalAddress.sin_family 		= AF_INET;
	m_LocalAddress.sin_addr.s_addr 	= htonl(INADDR_ANY);
	m_LocalAddress.sin_port 		= htons(Port);

	int rc = bind (m_Socket, (struct sockaddr *)&m_LocalAddress, m_SocketAddressSize);
	if(rc<0) 
	{
		printf("cannot bind port number %d \n", Port);
		return 0;
	}

	return 1;	
}

int CUDPSocket::Receive(char * Buffer)
{
	int n = recvfrom(m_Socket, Buffer, 100, 0, (struct sockaddr *)&m_RemoteAddress, &m_SocketAddressSize);
	
	return n;	
}

int CUDPSocket::ReceiveFrom(char * Buffer, sockaddr_in& address)
{
	int n = recvfrom(m_Socket, Buffer, 100, 0, (struct sockaddr *)&address, &m_SocketAddressSize);
	
	return n;	
}

int CUDPSocket::Send(char * Buffer)
{
	int n = sendto(m_Socket, Buffer, 100, 0, (struct sockaddr *)&m_RemoteAddress, m_SocketAddressSize);
	
	return n;	
}

int CUDPSocket::SendTo(char * Buffer, sockaddr_in& address)
{
	int n = sendto(m_Socket, Buffer, 100, 0, (struct sockaddr *)&address, m_SocketAddressSize);
	
	return n;	
}

sockaddr_in CUDPSocket::GetDestinationAddress(void)
{
	return m_RemoteAddress;
}

sockaddr_in CUDPSocket::GetLocalAddress(void)
{
	return m_LocalAddress;
}

void CUDPSocket::SetDestinationAddress(char * IP, const int Port)
{
	m_RemoteAddress.sin_family = AF_INET;
	m_RemoteAddress.sin_port = htons (Port);
	m_RemoteAddress.sin_addr.s_addr = inet_addr (IP);
}

SOCKET CUDPSocket::GetSocket(){
	return m_Socket;
}
#ifndef CLIENT_H
#define CLIENT_H

#include "hsfsocket.h"

using namespace std;

#define CLIENT_UPDATE_PERIOD	1.0		// in seconds

class Client
{
public:
	Client(void);

	bool	Initialize();

	void	Update(float dt);
	void	SetClientTarget(Vector3f &posToTrack);	// sets the position to send updates about when sending data to server

	~Client(void);
	
private:
	void	SendToServer();							// send client data to the server

private:
	sockaddr_in		me;
	Vector3f		*posToSend;				// a pointer to a vector3f that holds the position data to send to the server
	CUDPSocket		*socket;
	ServerPacket	*recvPacket;			// the packet to receive from the server
	Packet			*packet;				// the packet to send to the server
	char			msgBuffer[BUFFERSIZE];

	int				serverPacketNum;		// keep track of the server packets coming in

	float			millis;
};

#endif
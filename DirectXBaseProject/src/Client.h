#ifndef CLIENT_H
#define CLIENT_H

#include "hsfsocket.h"

using namespace std;

#define CLIENT_UPDATE_PERIOD	0.1		// in seconds

class Client
{
public:
	Client(void);

	bool	Initialize(HWND hwnd);

	void	SetServerDetails();

	void	ProcessMessage(WPARAM msg, LPARAM lParam);
	void	Update(float dt);
	void	SetClientTarget(Vector3f &posToTrack);	// sets the position to send updates about when sending data to server

	~Client(void);
	
private:
	void	SendToServer();							// send client data to the server
	void	ProcessServerData(); 

private:
	sockaddr_in		me;
	Vector3f		*posToSend;				// a pointer to a vector3f that holds the position data to send to the server
	CUDPSocket		*socket;
	ServerPacket	recvPacket;			// the packet to receive from the server
	Packet			packet;				// the packet to send to the server
	char			msgBuffer[BUFFERSIZE];	// a buffer...for messages...what'd you expect?

	int				serverPacketNum;		// keep track of the server packets coming in

	float			millis;					// keep track of the milliseconds passed in order to send info to the server on a regular basis

	int				numPlayers;				// keep track of number of players in the game

	// contains the data necessary to establish a connection
	char			serverIP[20];
	int				portNum;
};

#endif
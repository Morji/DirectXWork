#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkController.h"
#include <vector>

using namespace std;

#define CLIENT_UPDATE_PERIOD	0.1f		// in seconds


/**
A client implementation using the windows message loop.
The server in this instance is a player in the game and 
sends updates about itself to clients as well
**/
class Client : public NetworkController
{
public:
	Client(void);

	bool	Initialize(HWND hwnd);

	void	SetServerDetails();

	void	ProcessMessage(WPARAM msg, LPARAM lParam);
	void	Update(float dt);
	
	~Client(void);
	
private:
	void	SendToServer();						// send client data to the server
	void	ProcessServerData(); 
	void	LerpPlayersPositions(float dt);
	void	ResetPlayerData();
private:
	ServerPacket		recvPacket;				// the packet to receive from the server
	Packet				packet;					// the packet to send to the server

	char				msgBuffer[CLIENT_BUFFERSIZE];	// a buffer...for messages...what'd you expect?
	char				packetBuffer[SERVER_BUFFERSIZE];
	int					serverPacketNum;		// keep track of the server packets coming in

	int					numPlayers;				// keep track of number of players in the game

	ClientData			*currentPlayerData;		// an array to hold the currently received client positions

	// contains the data necessary to establish a connection
	char				serverIP[20];
	int					portNum;
};

#endif
#ifndef SERVER_H
#define SERVER_H

#include "hsfsocket.h"

#include <vector>

#define			SERVER_UPDATE_PERIOD		0.1		// in seconds

using namespace std;

/**
A Server implementation using the windows message loop.
The server in this instance is a player in the game and 
sends updates about itself to clients as well
**/
class Server{
public:
	Server(void);
	~Server(void);

	bool			StartServer(HWND hwnd);
	void			ProcessMessage(WPARAM msg, LPARAM lParam);
	void			Update(float dt);

	void			SetTarget(Vector3f &posToTrack);	// the playable target of the server

private:
	void			AddClient(ClientInfo &clientInfo);
	bool			ClientExists(sockaddr_in &identifier);	//checks wether a client with this address exists in the collection of clients
	void			UpdateServer();
	void			UpdateClient(ClientInfo &clientInfo);	//updates the client associated with the current address

//Private members for the player operation
private:
	Vector3f					*posToSend;				// a pointer to a vector3f that holds the position data of the player
	int							serverId;				// the id of the server player
//Private members for server operation
private:
	sockaddr_in					me;
	vector<ClientInfo>			clientVector;			// The collection of clients connected to this server	

	CUDPSocket					*socket;
	ServerPacket				serverPacket;			// the game state packet to send to clients
	Packet						recvPacket;				// packet to handle receiving of any client data;

	char						msgBuffer[BUFFERSIZE];
	char						packetBuffer[BUFFERSIZE];
	//char						data[16];
	int							numConnections;			// Keep track of number of connections	
	int							serverPacketNum;

	float						millis;					// Keep track time to update server at regular intervals
};


#endif
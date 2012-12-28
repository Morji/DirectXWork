#ifndef SERVER_H
#define SERVER_H

#include "NetworkController.h"

#include <vector>

#define			SERVER_UPDATE_PERIOD		0.1f		// in seconds

using namespace std;

/**
A Server implementation using the windows message loop.
The server in this instance is a player in the game and 
sends updates about itself to clients as well
**/
class Server : public NetworkController
{
public:
	Server(void);
	~Server(void);

	bool			StartServer(HWND hwnd);
	void			ProcessMessage(WPARAM msg, LPARAM lParam);
	void			Update(float dt);

private:
	void			DisplayServerInfo();

	void			AddClient(ClientInfo &clientInfo);
	bool			ClientExists(int ID);	//checks wether a client with this ID exists in the collection of clients
	void			UpdateServer();
	void			UpdateClient(int position,PlayerData &data);	//updates the client associated with the current address
	void			DisconnectClientByPos(int position);
	void			DisconnectClientBySocket(SOCKET socket);

	void			LerpPlayersPositions(float dt);
	void			ResetPlayerData();

//Private members for the player operation
private:
	ClientData				serverPlayerData;		// the client data of the server player
//Private members for server operation
private:
	ServerPacket			serverPacket;			// the game state packet to send to clients
	Packet					recvPacket;				// packet to handle receiving of any client data;

	vector<ClientInfo>		clientVector;			// The collection of info of clients connected to this server

	char					msgBuffer[CLIENT_BUFFERSIZE];	// a buffer...for messages...what'd you expect?
	char					packetBuffer[SERVER_BUFFERSIZE];
	int						numConnections;			// Keep track of number of connections	
	int						serverPacketNum;
};


#endif
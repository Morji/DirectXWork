#ifndef SERVER_H
#define SERVER_H

#include "hsfsocket.h"

#include <vector>

#define			SERVER_UPDATE_PERIOD		0.1		//in seconds

using namespace std;

typedef struct ClientInfo{
	Vector3f	clientPos;
	sockaddr_in clientAddress;
}ClientInfo;

class Server{
public:
	Server(void);
	~Server(void);


	bool			StartServer();
	void			ProcessMessage(WPARAM msg, LPARAM lParam);
	void			Update(float dt);

private:
	void			AddClient(sockaddr_in &address,Vector3f &pos);
	bool			ClientExists(sockaddr_in &address);	//checks wether a client with this address exists in the collection of clients
	void			UpdateServer();
	void			UpdateClient(sockaddr_in &address, Vector3f &pos);	//updates the client associated with the current address

private:
	sockaddr_in					me;
	vector<ClientInfo>			clientVector;			// The collection of clients connected to this server
	int							numConnections;			// Keep track of number of connections
	char						packetBuffer[BUFFERSIZE];
	CUDPSocket					*socket;
	ServerPacket				*serverPacket;			// the game state packet to send to clients
	Packet						recvPacket;				// packet to handle receiving of any client data;
	char						msgBuffer[BUFFERSIZE];

	int							serverPacketNum;

	float						millis;
};


#endif
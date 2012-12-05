#ifndef NETWORKDEFINES_H
#define NETWORKDEFINES_H

#include <iostream>
#include "Vertex.h"
#include <vector>
// the ip address of the server to connect to
#define SERVERIP "127.0.0.1"

//the port of the server to connect to
#define SERVERPORT 5555
#define CLIENTPORT 5556

#define WM_SOCKET (WM_USER+1)	//handle socket messages in windows message loops

#define BUFFERSIZE		(100)
#define MSG_CONNECTED	(0x41)	//A
#define NUMCONN			(4)

///REMEMBER - MAX UDP PACKET SIZE IS ~ 512 BYTES TO MAKE SURE IT DOESN'T GET SPLIT UP

/* The client info struct is to be used by the server 
to keep a track of the clients and update each ones info
correctly - it is used internally and not sent over the network*/
typedef struct ClientInfo{
	Vector3f	clientPos;
	sockaddr_in clientAddress;
}ClientInfo;

/* The client data struct is used by the server(it contains an array of these) to send
the collective client information to each client. It is sent over the network, so it is
kept as small as possible*/
typedef struct ClientData{
	Vector3f	clientPos;
	int			clientID;
	
	ClientData(){}

	ClientData(Vector3f pos, int id){
		clientPos = pos;
		clientID = id;
	}
}ClientData;

struct Packet{
	int			ID;
	Vector3f	position;

	Packet(){
		ID = 0;
		position = Vector3f(0,0,0);
	}
};

struct ServerPacket{
	short				packetSize; // 2 bytes
	int					ID;			// 4 bytes
	//char				data[32];	// 32 bytes(2*ClientData)
									
	char				data[1];	// stretchy array - the size of it will be PlayerCount*sizeof(ClientData)

	ServerPacket(){
		ID = 0;
		packetSize = 0;
	}

	~ServerPacket(){
	}

	int GetPlayerCount(){
		return ((packetSize)-7)/sizeof(ClientData);
	}
};

#endif
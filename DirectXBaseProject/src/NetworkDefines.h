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

#define SERVER_BUFFERSIZE		(140)	//at least sizeof(ServerPacket) + maxPlayers*sizeof(ClientData)
#define CLIENT_BUFFERSIZE		(80)	//at least sizeof(ServerPacket) + maxPlayers*sizeof(ClientData)
#define MSG_CONNECTED			(0x41)	//A
#define NUMCONN					(5)

///REMEMBER - MAX UDP PACKET SIZE IS ~ 512 BYTES TO MAKE SURE IT DOESN'T GET SPLIT UP
typedef struct PlayerData{
	Vector3f	pos;	//12 bytes
	Vector3f	rot;	//12 bytes

	PlayerData(){
		pos = Vector3f(0,1.5f,0);
		rot = Vector3f(0,0,0);
	}

	PlayerData(Vector3f pos,Vector3f rot){
		this->pos = pos;
		this->rot = rot;
	}
}PlayerData;
/* The client data struct is used by the server(it contains an array of these) to send
the collective client information to each client. It is sent over the network, so it is
kept as small as possible*/
typedef struct ClientData{
	PlayerData	playerData;	//24 bytes
	int			clientID;	//4 bytes
	
	ClientData(){		
		clientID = -1;
	}

	ClientData(PlayerData &data, int id){
		playerData = data;
		clientID = id;
	}
}ClientData;

/* The client info struct is to be used by the server 
to keep a track of the clients and update each ones info
correctly - it is used internally and not sent over the network*/
typedef struct ClientInfo{
	sockaddr_in clientAddress;
	ClientData	client;

	ClientInfo(){}

	ClientInfo(sockaddr_in &clientAddress,ClientData &data){
		this->clientAddress = clientAddress;
		this->client		= data;
	}
}ClientInfo;

struct Packet{
	int			ID;
	PlayerData	data;

	Packet(){
		ID = 0;
	}
};

struct ServerPacket{
	short				numPlayers; // 2 bytes
	int					ID;			// 4 bytes
};

#endif
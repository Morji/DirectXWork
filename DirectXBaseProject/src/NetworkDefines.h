#ifndef NETWORKDEFINES_H
#define NETWORKDEFINES_H

#include <iostream>
#include "Vertex.h"

// the ip address of the server to connect to
#define SERVERIP "127.0.0.1"

//the port of the server to connect to
#define SERVERPORT 5555
#define CLIENTPORT 5556

#define WM_SOCKET (WM_USER+1)	//handle socket messages in windows message loops

#define BUFFERSIZE		(100)
#define MSG_CONNECTED	(0x41)	//A
#define NUMCONN			(4)

struct Packet{
	int			ID;
	Vector3f	position;

	Packet(){
		ID = 0;
		position = Vector3f(0,0,0);
	}
};

struct ServerPacket{
	int			ID;
	Vector3f	*positionData;

	ServerPacket(int clientSize){
		positionData = new Vector3f[clientSize];
	}

	~ServerPacket(){
		delete [] positionData;
		positionData = nullptr;
	}

	int	GetClientSize(){
		return sizeof(positionData)/sizeof(positionData[0]);
	}
};

#endif
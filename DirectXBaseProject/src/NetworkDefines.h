#ifndef NETWORKDEFINES_H
#define NETWORKDEFINES_H

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "Vertex.h"

#pragma comment(lib, "ws2_32.lib")

// the ip address of the server to connect to
#define SERVERIP "127.0.0.1"

//the port of the server to connect to
#define SERVERPORT 5555

#define WM_SOCKET (WM_USER+1)	//handle socket messages in windows message loops

#define BUFFERSIZE		(100)
#define MSG_CONNECTED	(0x41)	//A
#define TEXTSIZE		(80)
#define NUMCONN			(4)

struct Packet{
	int			ID;
	Vector3f	position;

	Packet(){
		ID = 0;
		position = Vector3f(0,0,0);
	}
};

#endif
#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include "hsfsocket.h"

/*
The base class that Client and Server inherit from. This provides the basic implementation of the 
required compononents for networked play.
*/

class NetworkController
{
public:
	NetworkController(void);

	bool		playerJoined;//track if a player has joined the game

	void		SetTarget(Vector3f &posToTrack, Vector3f &rotToTrack);	// sets the player to send updates about when sending data to server

	ClientData	*players;			// an array to hold the client data coming from the server - public for fast access

	virtual void ResetPlayerData()=0;

	~NetworkController(void);

protected:
	Vector3f			*posToSend;
	Vector3f			*rotToSend;
	CUDPSocket			socket;		
	float				millis;					// Keep track time to update network info at regular intervals
};

#endif
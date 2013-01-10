#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include "hsfsocket.h"
#include "ModelObject.h"
#include <list>
#include <vector>

using namespace std;

/*
The base class that Client and Server inherit from. This provides the basic implementation of the 
required compononents for networked play.
*/

class NetworkController
{
public:
	NetworkController(void);

	void		SetTarget(Vector3f &posToTrack, Vector3f &rotToTrack, bool &movementToTrack);	// sets the player to send updates about when sending data to server

	ClientData	*players;			// an array to hold the client data coming from the server - public for fast access

	void		InitNetworkController(ModelObject *baseModel, std::list<GameObject*> *masterList);//a base model to create the new players from and a list to store them in
	
	void		UpdatePlayers();
	

	std::vector<ModelObject*>	*GetPlayerList();

	virtual void ResetPlayerData()=0;

	~NetworkController(void);

protected:
	void		AddNewPlayer();
	void		DeletePlayer(int pos);
	
protected:
	Vector3f			*posToSend;
	Vector3f			*rotToSend;
	bool				*isMoving;
	CUDPSocket			socket;
	CUDPSocket			tcpSocket;

	float				millis;					// Keep track time to update network info at regular intervals

	std::vector<ModelObject*>	mPlayerVector;
	std::list<GameObject*>		*masterObjectList;
private:
	ModelObject			*mBaseModel;
};

#endif
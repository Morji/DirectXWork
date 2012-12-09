#include "NetworkController.h"


NetworkController::NetworkController(void){
	rotToSend = 0;
	posToSend = 0;
	millis = 0.0f;
	players = 0;
	playerJoined = false;
}


NetworkController::~NetworkController(void){
	rotToSend = nullptr;
	posToSend = nullptr;
	if (players){
		delete [] players;
		players = nullptr;
	}
}

void NetworkController::SetTarget(Vector3f &posToTrack, Vector3f &rotToTrack){
	posToSend = &posToTrack;
	rotToSend = &rotToTrack;
}
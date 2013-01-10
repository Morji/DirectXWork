#include "NetworkController.h"

NetworkController::NetworkController(void){
	rotToSend = 0;
	posToSend = 0;
	millis = 0.0f;
	players = 0;
	mBaseModel = 0;
	isMoving = 0;
	masterObjectList = 0;
}


NetworkController::~NetworkController(void){
	rotToSend = nullptr;
	posToSend = nullptr;
	isMoving = nullptr;
	if (players){
		delete [] players;
		players = nullptr;
	}
	mPlayerVector.clear();
	masterObjectList = nullptr;
	mBaseModel = nullptr;
	closesocket(tcpSocket.GetSocket());
	closesocket(socket.GetSocket());
	
}

void NetworkController::AddNewPlayer(){		
	ModelObject *copy = new ModelObject(*mBaseModel);
	copy->AddReference();
	copy->pos = Vector3f(0,1.5f,0);
	masterObjectList->push_back(copy);
	mPlayerVector.push_back(copy);
}

void NetworkController::DeletePlayer(int pos){
	list<GameObject*>::const_iterator iterator;
	for (iterator = masterObjectList->begin(); iterator != masterObjectList->end(); iterator++) {
		if ((*iterator) == mPlayerVector.at(pos)){
			delete (*iterator);	
			iterator = masterObjectList->erase(iterator);

			if (iterator == masterObjectList->end())
				break;
		}
	}
	mPlayerVector.erase(mPlayerVector.begin() + pos);	
}

vector<ModelObject*> *NetworkController::GetPlayerList(){
	return &mPlayerVector;
}

void NetworkController::UpdatePlayers(){
	for (int i = 0; i < mPlayerVector.size(); i++){
		mPlayerVector[i]->pos = players[i].playerData.pos;
		mPlayerVector[i]->theta = players[i].playerData.rot;
	}
}

void NetworkController::InitNetworkController(ModelObject *baseModel, std::list<GameObject*> *masterList){
	masterObjectList = masterList;
	mBaseModel = baseModel;
}

void NetworkController::SetTarget(Vector3f &posToTrack, Vector3f &rotToTrack, bool &movementToTrack){
	posToSend = &posToTrack;
	rotToSend = &rotToTrack;
	isMoving = &movementToTrack;

}
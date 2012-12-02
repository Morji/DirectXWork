#include "Client.h"


Client::Client(void){
	serverPacketNum = 0;
	millis = 0;
	posToSend = 0;
	numPlayers = 0;
}


Client::~Client(void){
	if (socket){
		delete socket;
		socket = nullptr;
	}
	if (packet){
		delete packet;
		packet = nullptr;
	}
	/*if (posToSend){
		delete posToSend;
		posToSend = nullptr;
	}*/
	WSACleanup();
}

bool Client::Initialize(HWND hwnd){

	socket = new CUDPSocket();

	packet = new Packet();
	if (!packet){
		cout << "Failure initializing socket" << endl;
		return false;
	}
	packet->ID = 0;

	if (!socket->Initialise()){
		cout << "Failure initializing socket" << endl;
		return false;
	}
	
	if (!socket->Bind(CLIENTPORT)){
		cout << "Failure initializing socket" << endl;
		return false;
	}	

	socket->SetDestinationAddress(serverIP,portNum);

	WSAAsyncSelect (socket->GetSocket(),hwnd,WM_SOCKET,( FD_READ ));

	if (WSAGetLastError() != 0)
		cout << "Client error message: " << WSAGetLastError() << endl;

	return true;
}

void Client::SetServerDetails(){
	cout << "Input the server IP address: ";
	cin >> serverIP;

	cout << endl << "Input the port number ( 5000 < input < 6000) (return for default): ";
	cin >> portNum;

	//make sure we have a valid IP address and port
	if (strlen(serverIP) < 0)
		strcpy_s(serverIP,SERVERIP);
	if (portNum < 5000 || portNum > 6000)
		portNum = SERVERPORT;
}

void Client::SetClientTarget(Vector3f &posToTrack){
	posToSend = &posToTrack;
}

void Client::Update(float dt){
	millis+= dt;
	if (millis >= CLIENT_UPDATE_PERIOD){
		SendToServer();
		millis = 0.0f;
	}
}

// Read the server packet and update client info as necessary
void Client::ProcessServerData(){
	/*if the server client size is different than the amount of a players
	a player has either connected or disconnected - act accordingly*/
	if (recvPacket.GetPlayerCount() != numPlayers){
		numPlayers = recvPacket.GetPlayerCount();
	}
	int byteStart = 0;
	int byteEnd = sizeof(ClientData);
	char *data = new char[sizeof(ClientData)];
	ClientData *playerInfo = new ClientData;
	for (int i = 0; i < numPlayers; i++){
		for (int j = byteStart; j < byteEnd; j++){
			data[j-byteStart] = recvPacket.clientData.at(j);		
		}		
		memcpy(playerInfo,data,sizeof(ClientData));
		cout << "Player with id: " << playerInfo->clientID << " new position: " << playerInfo->clientPos.x << "," << playerInfo->clientPos.y << "," << playerInfo->clientPos.z << endl;
		byteStart = byteEnd;
		byteEnd += sizeof(ClientData);
		memset(data,0x0,sizeof(ClientData));
	}

	//this deletes both pointers
	delete playerInfo;
	playerInfo = nullptr;

	delete [] data;
	data = nullptr;
}

// Processes any pending network messages
void Client::ProcessMessage(WPARAM msg, LPARAM lParam){
	if (WSAGETSELECTERROR(lParam)){
		cout << "Socket error\n";
	}

	switch (WSAGETSELECTEVENT(lParam)){

		case FD_READ:{			
			memset(msgBuffer, 0x0, BUFFERSIZE);//clear the buffer
			int bytes = socket->Receive(msgBuffer);
			//check for any errors on the socket
			if(bytes == SOCKET_ERROR){
				int SocketError = WSAGetLastError();
				printf("Client received socket error from %d with error:%d\n", msg, SocketError);
			}
			// We have received data so process it			
			if(bytes > 0){
				sockaddr_in clientAddress = socket->GetDestinationAddress();
				short packetSize = 0;
				memcpy(&packetSize, msgBuffer , 2);//get the packet size
				memcpy(&recvPacket,msgBuffer,packetSize);//copy the required amount into the packet
				if (recvPacket.ID > serverPacketNum){
					serverPacketNum = recvPacket.ID;
					ProcessServerData();
					
				}
			}
			break;}
	}
}

void Client::SendToServer(){
	packet->ID++;
	packet->position = *posToSend;
	memset(msgBuffer, 0x0, BUFFERSIZE);//clear the buffer
	memcpy(msgBuffer, packet , sizeof(Packet));
	socket->Send(msgBuffer);
}
#include "Client.h"


Client::Client(void){
	serverPacketNum = 0;
	numPlayers = 0;
	currentPlayerData = 0;
}


Client::~Client(void){
	if (currentPlayerData){
		delete [] currentPlayerData;
		currentPlayerData = nullptr;
	}
	WSACleanup();
}

bool Client::Initialize(HWND hwnd){

	if (!socket.Initialise()){
		cout << "Failure initializing socket" << endl;
		return false;
	}
	
	if (!socket.Bind(CLIENTPORT)){
		cout << "Failure initializing socket" << endl;
		return false;
	}	

	socket.SetDestinationAddress(serverIP,portNum);

	WSAAsyncSelect (socket.GetSocket(),hwnd,WM_SOCKET,( FD_READ ));

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

void Client::Update(float dt){
	millis+= dt;
	if (millis >= CLIENT_UPDATE_PERIOD){
		SendToServer();
		millis = 0.0f;
	}
	if (players){
		LerpPlayersPositions(millis);
	}
}

void Client::LerpPlayersPositions(float dt){
	for (int i = 0; i < numPlayers; i++){		
		D3DXVec3Lerp(&players[i].playerData.pos,&players[i].playerData.pos,&currentPlayerData[i].playerData.pos,dt);
		D3DXVec3Lerp(&players[i].playerData.rot,&players[i].playerData.rot,&currentPlayerData[i].playerData.rot,dt);
	}
}

// Read the server packet and update client info as necessary
void Client::ProcessServerData(){	
	/*if the server client size is different than the amount of a players
	a player has either connected or disconnected - act accordingly*/	
	if (recvPacket.numPlayers != numPlayers){
		numPlayers = recvPacket.numPlayers;
		delete [] currentPlayerData;
		currentPlayerData = new ClientData[numPlayers];
		playerJoined = true;
	}	
	short size = sizeof(ClientData)*recvPacket.numPlayers;//set the player data buffer to be as big as necessary
	memset(currentPlayerData,0x0,size);
	memcpy(currentPlayerData,packetBuffer+sizeof(ServerPacket),size);
	serverPacketNum = recvPacket.ID;	

	//a new player has joined/left so reset the player data
	if (playerJoined)
		ResetPlayerData();
}

void Client::ResetPlayerData(){
	delete [] players;
	players = new ClientData[numPlayers];

	for (int i = 0; i < numPlayers; i++){
		players[i].clientID = currentPlayerData[i].clientID;
		players[i].playerData.pos = currentPlayerData[i].playerData.pos;
		players[i].playerData.rot = currentPlayerData[i].playerData.rot;
	}
}

// Processes any pending network messages
void Client::ProcessMessage(WPARAM msg, LPARAM lParam){
	if (WSAGETSELECTERROR(lParam)){
		cout << "Socket error\n";
	}

	switch (WSAGETSELECTEVENT(lParam)){

		case FD_READ:{			
			memset(packetBuffer, 0x0, SERVER_BUFFERSIZE);//clear the buffer
			int bytes = socket.Receive(packetBuffer);
			//check for any errors on the socket
			if(bytes == SOCKET_ERROR){
				int SocketError = WSAGetLastError();
				printf("Client received socket error from %d with error:%d\n", msg, SocketError);
			}
			// We have received data so process it			
			if(bytes > 0){
				sockaddr_in clientAddress = socket.GetDestinationAddress();
				recvPacket = *(ServerPacket*)packetBuffer;				
				if (recvPacket.ID != serverPacketNum){					
					ProcessServerData();					
				}
			}
			break;}
	}
}

void Client::SendToServer(){
	packet.ID++;
	packet.data.pos = *posToSend;
	packet.data.rot = *rotToSend;
	memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
	memcpy(msgBuffer, &packet , sizeof(Packet));
	socket.Send(msgBuffer);
}
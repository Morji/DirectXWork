#include "Server.h"

Server::Server(){
	serverPacketNum = 0;
	numConnections = 0;
	millis = 0;
	serverPlayerData.clientID = 0;
}

Server::~Server(){
	while (!clientVector.empty()){
		clientVector.pop_back();
	}
	clientVector.clear();
	WSACleanup();
}

bool Server::StartServer(HWND hwnd){

	cout << "Starting server" << endl;
	if (!socket.Initialise()){
		cout << "Server failed to start" << endl;
		return false;
	}

	if (!socket.Bind(SERVERPORT)){
		cout << "Server failed to start" << endl;
		return false;
	}

	WSAAsyncSelect (socket.GetSocket(),hwnd,WM_SOCKET,( FD_READ ));

	if (WSAGetLastError() != 0)
		cout << "Server error message: " << WSAGetLastError() << endl;

	return true;
}

void Server::AddClient(ClientInfo &clientInfo){
	//add client to vector of client info
	cout << "New client connected! IP: " << inet_ntoa(clientInfo.clientAddress.sin_addr) << endl;
	playerJoined = true;
	clientVector.push_back(clientInfo);
	ResetPlayerData();//new player has joined so reset the player data
}

void Server::UpdateClient(ClientInfo &clientInfo){
	for (int i = 0; i < numConnections; i++){
		if (clientVector[i].clientAddress.sin_addr.S_un.S_addr == clientInfo.clientAddress.sin_addr.S_un.S_addr){
			clientVector[i].client.playerData.pos = clientInfo.client.playerData.pos;
			clientVector[i].client.playerData.rot = clientInfo.client.playerData.rot;
			return;
		}
	}
}

void Server::LerpPlayersPositions(float dt){
	for (int i = 0; i < clientVector.size(); i++){
		D3DXVec3Lerp(&players[i].playerData.pos,&players[i].playerData.pos,&clientVector[i].client.playerData.pos,dt);
		D3DXVec3Lerp(&players[i].playerData.rot,&players[i].playerData.rot,&clientVector[i].client.playerData.rot,dt);
	}
}

//Send all client info to every client
void Server::UpdateServer(){

	memset(packetBuffer, 0x0, SERVER_BUFFERSIZE);//clear the buffer

	serverPacket.ID = ++serverPacketNum;
	serverPacket.numPlayers = numConnections+1;//server is also playing as well
	memcpy(packetBuffer, &serverPacket , sizeof(ServerPacket));//put the server packet info in first

	serverPlayerData.playerData.pos = *posToSend;
	serverPlayerData.playerData.rot = *rotToSend;
	//put the server player info into the packet - ID of 0
	memcpy(packetBuffer+sizeof(ServerPacket),&serverPlayerData,sizeof(ClientData));

	for (int i = 0; i < numConnections;i++){
		memcpy(packetBuffer+sizeof(ServerPacket)+(i+1)*sizeof(ClientData),&clientVector[i].client,sizeof(ClientData));
	}

	// send the server packet to all players
	for (int i = 0; i < numConnections; i++){
		//send update info of all players to client	
		socket.SendTo(packetBuffer,clientVector[i].clientAddress);				
	}
}

//Updates the server every update period
void Server::Update(float dt){
	millis+= dt;
	if (millis >= SERVER_UPDATE_PERIOD){
		UpdateServer();
		millis = 0.0f;
	}
	if (players)
		LerpPlayersPositions(millis);
}

void Server::ResetPlayerData(){
	delete [] players;
	players = new ClientData[numConnections];

	for (int i = 0; i < numConnections; i++){
		players[i].clientID = clientVector[i].client.clientID;
		players[i].playerData.pos = clientVector[i].client.playerData.pos;
		players[i].playerData.rot = clientVector[i].client.playerData.rot;
	}
}

// Processes any pending network messages
void Server::ProcessMessage(WPARAM msg, LPARAM lParam){
	if (WSAGETSELECTERROR(lParam)){
		cout << "Socket error\n";
	}

	switch (WSAGETSELECTEVENT(lParam)){

		case FD_READ:{
			
			memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
			int bytes = socket.Receive(msgBuffer);
			//check for any errors on the socket
			if(bytes == SOCKET_ERROR){
				int SocketError = WSAGetLastError();
				printf("Socket servicing client %d has error:%d\n", msg, SocketError);
				numConnections--;			    // decrement number of current connections
			}
			// We have received data so process it			
			if(bytes > 0){
				sockaddr_in clientAddress = socket.GetDestinationAddress();
				//cout << "Received data from " << inet_ntoa(clientAddress.sin_addr) << endl;					
				memcpy(&recvPacket, msgBuffer , sizeof(recvPacket));
				//if a new client has connected - add him
				if (!ClientExists(clientAddress)){
					if (numConnections+1 <= NUMCONN){
						numConnections++;	
						delete [] players;
						players = new ClientData[numConnections];
						ClientInfo info = ClientInfo(clientAddress,ClientData(recvPacket.data,numConnections));
						AddClient(info);
					}
				}
				//else if a connected client is sending updates - update him
				else{
					ClientInfo info = ClientInfo(clientAddress,ClientData(recvPacket.data,numConnections));
					UpdateClient(info);
				}
			}
			break;}
	}
}

bool Server::ClientExists(sockaddr_in &address){
	for (int i = 0; i < numConnections; i++){
		if (clientVector.at(i).clientAddress.sin_addr.S_un.S_addr == address.sin_addr.S_un.S_addr)
			return true;
	}
	return false;
}
#include "Server.h"

Server::Server(){
	socket = new CUDPSocket();
	serverPacketNum = 0;
	numConnections = 0;
	millis = 0;
}

Server::~Server(){
	if (socket){
		delete socket;
		socket = nullptr;
	}
	if (serverPacket){
		delete serverPacket;
		serverPacket = nullptr;
	}
	WSACleanup();
}

bool Server::StartServer(){

	serverPacket = new ServerPacket(numConnections);

	cout << "Starting server" << endl;
	if (!socket->Initialise()){
		cout << "Server failed to start" << endl;
		return false;
	}

	return true;
}

void Server::AddClient(sockaddr_in &address,Vector3f &pos){
	//add client to vector of client info
	ClientInfo info;
	info.clientAddress = address;
	info.clientPos = pos;
	
	clientVector.push_back(info);
}

void Server::UpdateClient(sockaddr_in &address,Vector3f &pos){
	for (int i = 0; i < numConnections; i++){
		if (clientVector.at(i).clientAddress.sin_addr.S_un.S_addr == address.sin_addr.S_un.S_addr){
			clientVector.at(i).clientPos = pos;
			cout << "Updating client pos to: " << pos.x << ", " << pos.y << "," << pos.z;
			return;
		}
	}
}

//Send all client info to every client
void Server::UpdateServer(){

	//only create a new version of the server packet if the client size has changed
	if (numConnections != serverPacket->GetClientSize()){
		delete serverPacket;
		serverPacket = nullptr;
		serverPacket = new ServerPacket(numConnections);
	}
	
	serverPacket->ID = ++serverPacketNum;

	// place the position data in the packet
	for (int i = 0; i < numConnections; i++){
		serverPacket->positionData[i] = clientVector.at(i).clientPos;
	}

	memset(packetBuffer, 0x0, BUFFERSIZE);//clear the buffer
	memcpy(serverPacket, packetBuffer , sizeof(ServerPacket));

	// send the server packet to all players
	for (int i = 0; i < numConnections; i++){
		//send update info of all players to client	
		socket->SendTo(packetBuffer,clientVector.at(i).clientAddress);				
	}
}

//Updates the server every update period
void Server::Update(float dt){
	millis+= dt;
	if (millis >= SERVER_UPDATE_PERIOD){
		UpdateServer();
		millis = 0.0f;
	}
}

// Processes any pending network messages
void Server::ProcessMessage(WPARAM msg, LPARAM lParam){
	if (WSAGETSELECTERROR(lParam)){
		cout << "Socket error\n";
	}

	switch (WSAGETSELECTEVENT(lParam)){

		case FD_READ:{
			
			int bytes = socket->Receive(msgBuffer);
			//check for any errors on the socket
			if(bytes == SOCKET_ERROR){
				int SocketError = WSAGetLastError();
				printf("Socket servicing client %d has error:%d\n", msg, SocketError);
				numConnections--;			    // decrement number of current connections
			}
			// We have received data so process it			
			if(bytes > 0){
				sockaddr_in clientAddress = socket->GetDestinationAddress();
				cout << "Received data from " << inet_ntoa(clientAddress.sin_addr);				
				memcpy(&recvPacket, msgBuffer, sizeof(recvPacket));
				//if a new client has connected - add him
				if (!ClientExists(clientAddress)){
					cout << "New client connected! " << endl;
					numConnections++;
					AddClient(clientAddress,recvPacket.position);
				}
				//else if a connected client is sending updates - update him
				else{
					UpdateClient(clientAddress,recvPacket.position);
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
#include "Server.h"

Server::Server(){
	socket = new CUDPSocket();
	serverPacketNum = 0;
	numConnections = 0;
	millis = 0;
	serverId = 0;
	posToSend = 0;
}

Server::~Server(){
	if (socket){
		delete socket;
		socket = nullptr;
	}
	/*if (posToSend){
		delete posToSend;
		posToSend = nullptr;
	}*/
	while (!clientVector.empty()){
		clientVector.pop_back();
	}
	clientVector.clear();
	WSACleanup();
}

bool Server::StartServer(HWND hwnd){

	cout << "Starting server" << endl;
	if (!socket->Initialise()){
		cout << "Server failed to start" << endl;
		return false;
	}

	if (!socket->Bind(SERVERPORT)){
		cout << "Server failed to start" << endl;
		return false;
	}

	WSAAsyncSelect (socket->GetSocket(),hwnd,WM_SOCKET,( FD_READ ));

	if (WSAGetLastError() != 0)
		cout << "Server error message: " << WSAGetLastError() << endl;

	serverId = socket->GetLocalAddress().sin_addr.S_un.S_addr;

	return true;
}

void Server::SetTarget(Vector3f &posToTrack){
	posToSend = &posToTrack;
}

void Server::AddClient(ClientInfo &clientInfo){
	//add client to vector of client info
	cout << "New client connected! IP: " << inet_ntoa(clientInfo.clientAddress.sin_addr) << endl;

	clientVector.push_back(clientInfo);
}

void Server::UpdateClient(ClientInfo &clientInfo){
	for (int i = 0; i < numConnections; i++){
		if (clientVector.at(i).clientAddress.sin_addr.S_un.S_addr == clientInfo.clientAddress.sin_addr.S_un.S_addr){
			clientVector.at(i).clientPos = clientInfo.clientPos;
			//cout << "Updating client pos to: " << clientInfo.clientPos.x << ", " << clientInfo.clientPos.y << "," << clientInfo.clientPos.z << endl;
			return;
		}
	}
}

//Send all client info to every client
void Server::UpdateServer(){

	memset(serverPacket.data,0x0,sizeof(ClientData)*(numConnections+1));
	serverPacket.ID = ++serverPacketNum;
	//position 0 is always the server	
	memcpy(serverPacket.data,&ClientData(*posToSend,serverId),sizeof(ClientData));

	for (int i = 0; i < numConnections; i++){
		memcpy(serverPacket.data+(i+1)*sizeof(ClientData),&ClientData(clientVector.at(i).clientPos,i+1),sizeof(ClientData));
	}

	serverPacket.packetSize = 7 + sizeof(ClientData)*(numConnections+1);

	memset(packetBuffer, 0x0, BUFFERSIZE);//clear the buffer
	memcpy(packetBuffer, &serverPacket , serverPacket.packetSize);

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
			
			memset(msgBuffer, 0x0, BUFFERSIZE);//clear the buffer
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
				//cout << "Received data from " << inet_ntoa(clientAddress.sin_addr) << endl;					
				memcpy(&recvPacket, msgBuffer , sizeof(recvPacket));
				//if a new client has connected - add him
				ClientInfo info = {recvPacket.position,clientAddress};
				if (!ClientExists(clientAddress)){
					numConnections++;					
					AddClient(info);
				}
				//else if a connected client is sending updates - update him
				else{
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
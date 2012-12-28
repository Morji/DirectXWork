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
	if (!socket.Initialise(UDP) || !tcpSocket.Initialise(TCP)){
		cout << "Server failed to start" << endl;
		return false;
	}

	if (!socket.Bind(SERVERPORT) || !tcpSocket.Bind(SERVERPORT-1)){
		cout << "Server failed to start" << endl;
		return false;
	}

	//enable listening on TCP socket
	if (listen(tcpSocket.GetSocket(),1)==SOCKET_ERROR){
		printf("Error:  Unable to listen!\n");
		WSACleanup ();
	}
	

	WSAAsyncSelect (socket.GetSocket(),hwnd,WM_SOCKET,( FD_READ ));//UDP socket
	WSAAsyncSelect (tcpSocket.GetSocket(),hwnd,WM_SOCKET,(FD_CLOSE | FD_ACCEPT | FD_READ ));//TCP socket

	if (WSAGetLastError() != 0)
		cout << "Server error message: " << WSAGetLastError() << endl;
	
	DisplayServerInfo();

	return true;
}

void Server::DisplayServerInfo(){
	//Display IP Address
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        cerr << "Error " << WSAGetLastError() <<
                " when getting local host name." << endl;
    }
	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
        cerr << "Error while looking up host IP address" << endl;
    }
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        cout << "Your IP address is: " << inet_ntoa(addr) << endl;
    }

	cout << "Your Port Number is: " << SERVERPORT << endl;
}

void Server::AddClient(ClientInfo &clientInfo){
	//add client to vector of client info
	cout << "New client connected! "; //<< inet_ntoa(clientInfo.clientAddress.) << endl;
	clientVector.push_back(clientInfo);
	AddNewPlayer();
	ResetPlayerData();//new player has joined so reset the player data
}

void Server::UpdateClient(int position,PlayerData &data){
	clientVector[position].client.playerData.pos = data.pos;
	clientVector[position].client.playerData.rot = data.rot;
	clientVector[position].timeSinceLastPacket = 0.0f;	
	
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

	serverPlayerData.clientID		= 0;
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
		clientVector[i].timeSinceLastPacket += millis;//increment time since last packet received for each player
		//if no data has been received for more than 3 seconds from a client - disconnect him
		if (clientVector[i].timeSinceLastPacket >= 3.0f){
			DisconnectClientByPos(i);
		}
	}
}

void Server::DisconnectClientByPos(int position){
	//inform all clients that this one has disconnected
	ConnPacket connPacket;
	connPacket.connected = false;	
	connPacket.clientID = clientVector[position].client.clientID;

	closesocket(clientVector[position].serviceSocket);

	clientVector.erase(clientVector.begin() + position);
	
	
	numConnections--;
	DeletePlayer(connPacket.clientID-1);
	ResetPlayerData();
	closesocket (clientVector[position].serviceSocket);
	memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
	memcpy(msgBuffer,&connPacket,sizeof(ConnPacket));
	for (int i = 0; i < numConnections; i++){
		tcpSocket.SendTo(msgBuffer,clientVector[i].clientAddress);
	}
}

void Server::DisconnectClientBySocket(SOCKET socket){
	//inform all clients that this one has disconnected
	ConnPacket connPacket;
	connPacket.connected = false;	
	for (int i = 0; i < numConnections; i++){
		if (clientVector[i].serviceSocket == socket){
			connPacket.clientID = clientVector[i].client.clientID;
			clientVector.erase(clientVector.begin() + i);
			break;
		}
	}
	
	closesocket (socket);
	numConnections--;
	DeletePlayer(connPacket.clientID-1);
	ResetPlayerData();
	memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
	memcpy(msgBuffer,&connPacket,sizeof(ConnPacket));
	for (int i = 0; i < numConnections; i++){
		tcpSocket.SendTo(msgBuffer,clientVector[i].clientAddress);
	}
	//clientVector.erase(clientVector.begin() + position);
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

		case FD_ACCEPT:{
			int Accepted;
			Accepted = 0;
			SOCKET newSocket;
			int sa_size = tcpSocket.GetSASize();
			if (numConnections < NUMCONN){						
				newSocket = accept (tcpSocket.GetSocket(),tcpSocket.GetTCPDestAddress(),&sa_size);
				if (msg==INVALID_SOCKET){
					cout << "Error:  Unable to accept connection!" << endl;
					WSACleanup ();
				}

				cout << "Accepted Client on socket " << newSocket << endl;
				Accepted = 1;
			}

			ConnPacket connPacket;
			memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
			if (Accepted){
				cout << "Client has connected!" << endl;
				numConnections++;				
				connPacket.connected = true;
				connPacket.clientID = numConnections;
				cout << "ClientID is: " << numConnections << endl;
				memcpy(msgBuffer, &connPacket, sizeof(ConnPacket));	
				send (newSocket, msgBuffer, CLIENT_BUFFERSIZE, 0);	

				//add new client to game
				delete [] players;
				players = new ClientData[numConnections];
				ClientInfo info = ClientInfo(newSocket,*tcpSocket.GetTCPDestAddress(),ClientData(numConnections));
				AddClient(info);
			}
			else{
				connPacket.connected = false;
				cout << "Can't accept any more connections" << endl;
				// Accept them immediately close the connection
				newSocket = accept (tcpSocket.GetSocket(),tcpSocket.GetTCPDestAddress(),&sa_size);
				memcpy(msgBuffer, &connPacket, sizeof(ConnPacket));	
				send (newSocket, msgBuffer, CLIENT_BUFFERSIZE, 0);	
				closesocket (newSocket);
			}
			
			break;}

		case FD_CLOSE:{
				cout << "Client on socket " << msg << " has quit " << endl;
				DisconnectClientBySocket(msg);
				break;}

		case FD_READ:{			
			memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
			int bytes = socket.Receive(msgBuffer);
			//check for any errors on the socket
			if(bytes == SOCKET_ERROR){
				int SocketError = WSAGetLastError();
				printf("Socket servicing client %d has error:%d\n", msg, SocketError);
				//numConnections--;			    // decrement number of current connections
			}
			// We have received data so process it			
			if(bytes > 0){
				sockaddr_in clientAddress = socket.GetDestinationAddress();
				//cout << "Received data from " << inet_ntoa(clientAddress.sin_addr) << endl;					
				memcpy(&recvPacket, msgBuffer , sizeof(recvPacket));
				UpdateClient(recvPacket.ID-1,recvPacket.data);//-1 because server is ID 0
			}
			break;}
	}
}

bool Server::ClientExists(int ID){
	for (int i = 0; i < numConnections; i++){
		if (clientVector[i].client.clientID == ID)
			return true;
	}
	return false;
}
#include "Client.h"


Client::Client(void){
	serverPacketNum = 0;
	numPlayers = 0;
	currentPlayerData = 0;
	timeSinceLastConnAttempt = 0.0f;
	playerID = -1;
	connectedToServer = false;
}


Client::~Client(void){
	if (currentPlayerData){
		delete [] currentPlayerData;
		currentPlayerData = nullptr;
	}
	WSACleanup();
}

bool Client::Initialize(HWND hwnd){

	if (!socket.Initialise(UDP) || !tcpSocket.Initialise(TCP)){
		cout << "Failure initializing socket" << endl;
		return false;
	}
	
	if (!socket.Bind(CLIENTPORT) || !tcpSocket.Bind(CLIENTPORT)){
		cout << "Failure initializing socket" << endl;
		return false;
	}	

	socket.SetDestinationAddress(serverIP,portNum);
	tcpSocket.SetDestinationAddress(serverIP,portNum-1);

	WSAAsyncSelect (socket.GetSocket(),hwnd,WM_SOCKET,( FD_READ ));
	WSAAsyncSelect (tcpSocket.GetSocket(),hwnd,WM_SOCKET,(FD_CLOSE | FD_CONNECT | FD_READ ));

	if (WSAGetLastError() != 0)
		cout << "Client error message: " << WSAGetLastError() << endl;

	ConnectToServer();

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

bool Client::ConnectToServer(){	
	if(!connectedToServer){
		cout << "Connecting to server..." << endl;
		int error = connect(tcpSocket.GetSocket(),(LPSOCKADDR)&tcpSocket.GetDestinationAddress(),tcpSocket.GetSASize());
		if (error == SOCKET_ERROR){
			if (error != 10035)
				cout << "Error while connecting: " << WSAGetLastError() << endl;
		}
	}
	else{
		cout << "Already connected to server" << endl;
	}
	return false;
}

void Client::Update(float dt){
	millis+= dt;
	if (connectedToServer){
		if (millis >= CLIENT_UPDATE_PERIOD){
			SendToServer();
			millis = 0.0f;
		}
		if (players){
			LerpPlayersPositions(millis);
		}
	}
	else{
		timeSinceLastConnAttempt += millis;
		if (millis >= SERVER_CONN_WAIT_PERIOD){
			ConnectToServer();
			millis = 0.0f;
		}
	}
}

void Client::LerpPlayersPositions(float dt){
	if (dt > 1.0f)
		dt = 1.0f;
	for (int i = 0; i < numPlayers; i++){		
		D3DXVec3Lerp(&players[i].playerData.pos,&players[i].playerData.pos,&currentPlayerData[i].playerData.pos,dt);
		D3DXVec3Lerp(&players[i].playerData.rot,&players[i].playerData.rot,&currentPlayerData[i].playerData.rot,dt);
	}
}

// Read the server packet and update client info as necessary
void Client::ProcessServerData(){		
	short size = sizeof(ClientData)*numPlayers;//set the player data buffer to be as big as necessary
	memset(currentPlayerData,0x0,size);
	memcpy(currentPlayerData,packetBuffer+sizeof(ServerPacket),size);
	serverPacketNum = recvPacket.ID;		
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
		cout << WSAGetLastError() << "\n";
		cout << WSAGETSELECTERROR(lParam) << "\n";
		return;
	}
	
	switch (WSAGETSELECTEVENT(lParam)){

		case FD_CONNECT:{
			//if a valid connection is made to the server
			//the  MSG_CONNECTED message will be recieved.
			cout << "FD_CONNECT called" << endl;			
			break;}

		case FD_CLOSE:{
			
			cout << "Disconnected from server! Proceeding in single player, attempting to re-establish connection" << endl;
			closesocket(tcpSocket.GetSocket());
			tcpSocket.Initialise(TCP);
			connectedToServer = false;
			break;}

		case FD_READ:{
			//determine which socket the message came from and act accordingly
			//on UDP socket
			if (msg == socket.GetSocket()){
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
			}
			//on TCP socket
			else if (msg == tcpSocket.GetSocket()){
				memset(msgBuffer, 0x0, SERVER_BUFFERSIZE);//clear the buffer
				int bytes = tcpSocket.Receive(msgBuffer);
				//check for any errors on the socket
				if(bytes == SOCKET_ERROR){
					int SocketError = WSAGetLastError();
					printf("Client received socket error from %d with error:%d\n", msg, SocketError);
				}
				// We have received data so process it			
				if(bytes > 0){
					//if not connected to server this means server has processed our connection attempt and has designated an ID
					if (!connectedToServer){
						ConnPacket connPacket;
						connPacket = *(ConnPacket*)msgBuffer;
						if (connPacket.connected == true){					
							playerID = connPacket.clientID;//get ID assigned from server
							connectedToServer = true;
							cout << "Connected to server, playerID is: " << playerID << endl;
							numPlayers = playerID+1;//server is ID 0 so number of players will be playerID + 1
							delete [] currentPlayerData;
							currentPlayerData = new ClientData[numPlayers];
							ResetPlayerData();
							for (int i = 0; i < playerID; i++)
								AddNewPlayer();
						}
						else{
							cout << "Server rejected connection " << endl;
						}
					}
					//otherwise it's a message from the server telling us a client has DC-ed or connected
					else{
						ConnPacket connPacket;
						connPacket = *(ConnPacket*)msgBuffer;						
						
						if (connPacket.connected == true){
							numPlayers++;
							cout << "New player joined game, playerID is: " << connPacket.clientID << endl;
							AddNewPlayer();
						}
						else{
							numPlayers--;
							DeletePlayer(connPacket.clientID-1);
							players[connPacket.clientID].clientID = -1;//alert game that a player has DC-ed
							cout << "Player with ID " << connPacket.clientID << " has disconnected" << endl;
						}						
						ResetPlayerData();
					}
				}
			}
			break;}
	}
}

void Client::SendToServer(){
	packet.ID = playerID;
	packet.data.pos = *posToSend;
	packet.data.rot = *rotToSend;
	memset(msgBuffer, 0x0, CLIENT_BUFFERSIZE);//clear the buffer
	memcpy(msgBuffer, &packet , sizeof(Packet));
	socket.Send(msgBuffer);
}
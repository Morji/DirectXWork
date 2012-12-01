#include "Client.h"


Client::Client(void){
	serverPacketNum = 0;
	millis = 0;
	posToSend = 0;
}


Client::~Client(void){
	if (socket){
		delete socket;
		socket = nullptr;
	}
	if (recvPacket){
		delete recvPacket;
		recvPacket = nullptr;
	}
	if (packet){
		delete packet;
		packet = nullptr;
	}
	if (posToSend){
		delete posToSend;
		posToSend = nullptr;
	}
	WSACleanup();
}

bool Client::Initialize(){

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

	char serverIP[20];
	int portNum;
	printf("Input the server IP address (return for default): ");
	cin >> serverIP;

	cout << endl << "Input the port number ( 5000 < input < 6000) (return for default): ";
	cin >> portNum;

	//make sure we have a valid IP address and port
	if (strlen(serverIP) < 0)
		strcpy_s(serverIP,SERVERIP);
	if (portNum < 5000 || portNum > 6000)
		portNum = SERVERPORT;

	socket->SetDestinationAddress(serverIP,portNum);

	return true;
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

void Client::SendToServer(){
	packet->ID++;
	packet->position = *posToSend;
	memset(msgBuffer, 0x0, BUFFERSIZE);//clear the buffer
	memcpy(msgBuffer, packet , sizeof(Packet));
	socket->Send(msgBuffer);
}
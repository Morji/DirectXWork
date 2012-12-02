//=============================================================================
// MainApp.cpp based on Frank Lunas Colored Cube App
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include "d3dApp.h"
#include "GameObject.h"
#include "CubeObject.h"
#include "Shader.h"
#include "GameCamera.h"
#include "Light.h"
#include "LightShader.h"
#include "TexShader.h"
#include "Grid.h"
#include "ModelObject.h"
#include "console.h"
#include "Server.h"
#include "Client.h"
#include <list>

using namespace std;

enum gameMode_t {SINGLE,CLIENT,SERVER};

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	~MainApp();
	int				scale;

	gameMode_t		gameMode;

	void processInput(float dt);

	void initGame();
	void initApp();
	void initServer();
	void initClient();

	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	void mouseScroll(int amount);

	LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);
	
private:
	void buildTransparentBS();
	void animateLights();
	void SwitchCameras();
	void MouseInput();	
	void initCameras();
	void initModels();
	void initGrids();
	void initShaders();

	void drawGrids();
	void drawModels();

private:
	ID3D10BlendState*		transparentBS;

	std::list<Shader*>		shaderList;
	std::list<GameObject*>	gameObjectList;
	std::list<GameCamera*>	gameCameraList;

	Light			light[3]; // 0 (parallel), 1 (point), 2 (spot)
	LIGHT_TYPE		lightType; // 0 (parallel), 1 (point), 2 (spot)

	ModelObject		*model;
	
	Grid			*water;
	Grid			*terrain;

	GameCamera		*godCamera;
	GameCamera		*playerCamera;
	GameCamera		*currentCam;

	TexShader		*texShader;
	TexShader		*multiTexShader;
	LightShader		*lightShader;
	Shader			*colorShader;

	float			aspectRatio;

	D3DXMATRIX		mView;
	D3DXMATRIX		mProj;
	D3DXMATRIX		mWVP;

	POINT			mouseLastPoint;
//server stuff
private:
	Server			*server;
	Client			*client;
};

LRESULT MainApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam){
	D3DApp::msgProc(msg,wParam,lParam);
	switch( msg ){
		case WM_SOCKET:
	// Get network messages
		switch (gameMode){			
			case SERVER:
				server->ProcessMessage(wParam, lParam);
				break;
			case CLIENT:
				client->ProcessMessage(wParam, lParam);
				break;
		}
		
		break;
	}

	return DefWindowProc(mhMainWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	ShowWin32Console();

	MainApp theApp(hInstance);

	theApp.initGame();	

	theApp.D3DApp::initApp();
	
	cout << "Loading up game..." << endl;
	theApp.initApp();	

	switch (theApp.gameMode){
	case SERVER:
		theApp.initServer();
		break;
	case CLIENT:
		theApp.initClient();
		break;
	}

	cout << "Starting game..." << endl;

	return theApp.run();
}

MainApp::MainApp(HINSTANCE hInstance)
: D3DApp(hInstance)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 

	aspectRatio = 0.5f;

	lightType = L_PARALLEL;//start light type is parallel

	// Parallel light.
	light[0].dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	light[0].ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	light[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light[0].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	// Point light--position is changed every frame to animate.
	light[1].ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	light[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light[1].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light[1].att.x = 0.0f;
	light[1].att.y = 0.1f;
	light[1].att.z = 0.0f;
	light[1].range = 50.0f;
	// Spotlight--position and direction changed every frame to animate.
	light[2].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	light[2].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light[2].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light[2].att.x    = 1.0f;
	light[2].att.y    = 0.0f;
	light[2].att.z    = 0.0f;
	light[2].spotPow  = 64.0f;
	light[2].range    = 10000.0f;

	//initialize variables to null
	model = NULL;
	terrain = NULL;
	lightShader = NULL;	
	texShader = NULL;
	colorShader = NULL;
	client =  NULL;
	server = NULL;
}

MainApp::~MainApp(){
	if( md3dDevice )
		md3dDevice->ClearState();

	//delete every shader in the list
	while (!shaderList.empty()){
		Shader* shader = shaderList.back();
		if (shader){
			shader->Shutdown();
			delete shader;
			shader = nullptr;
		}
		shaderList.pop_back();
	}
	shaderList.clear();

	//delete every game object in the list
	while (!gameObjectList.empty()){
		GameObject* object = gameObjectList.back();
		if (object){
			object->Shutdown();
			delete object;
			object = nullptr;
		}
		gameObjectList.pop_back();
	}
	gameObjectList.clear();

	//delete every camera object in the list
	while (!gameCameraList.empty()){
		GameCamera* object = gameCameraList.back();
		if (object){
			delete object;
			object = nullptr;
		}
		gameCameraList.pop_back();
	}
	gameCameraList.clear();

	if (server){
		delete server;
		server = nullptr;
	}

	if (client){
		delete client;
		client = nullptr;
	}
}

void MainApp::buildTransparentBS(){
	D3D10_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	HR(md3dDevice->CreateBlendState(&blendDesc, &transparentBS));
}

void MainApp::initGame(){
	char decision;
	cout << "Welcome to Val's very first DirectX and Server gig" << endl;
	cout << "Are you playing multi or single player?" << endl;
	cout << "Multi-player y/n: ";
	cin >> decision;
	if (decision != 'y'){
		gameMode = SINGLE;
		return;
	}
	cout << "Would you like to start as a server or a client? " << endl;
	cout << "Start as a server y/n: ";
	cin >> decision;
	if (decision == 'y'){
		gameMode = SERVER;		
	}
	else{
		gameMode = CLIENT;
		client = new Client();
		client->SetServerDetails();
	}
}

void MainApp::initApp(){	
	buildTransparentBS();

	initCameras();
	initModels();
	initGrids();
	initShaders();	

	
}

void MainApp::initServer(){
	server = new Server();
	if (!server->StartServer(getMainWnd())){
		cout << "Server Failed to start" << endl;
	}
	else
		server->SetTarget(model->pos);
}

void MainApp::initClient(){
	if (client){
		if (!client->Initialize(getMainWnd())){
			cout << "Client failed to start" << endl;
		}
		else
			client->SetClientTarget(model->pos);
	}
}

void MainApp::initCameras(){
	// Create the god camera object.
	godCamera = new GameCamera();
	// Set the initial position of the camera.
	godCamera->SetPosition(0.0f, 0.0f, -10.0f);
	gameCameraList.push_back(godCamera);

	// Create the player camera object.
	playerCamera = new GameCamera(true);
	playerCamera->SetPivotPoint(Vector3f(0,15,-15));
	// Set the initial position of the camera.
	gameCameraList.push_back(playerCamera);

	// make the current camera to be the god cam
	currentCam = playerCamera;
}

void MainApp::initModels(){
	bool result;

	model = new ModelObject();

	result = model->InitializeWithTexture(md3dDevice,L"assets/models/Grunt/grunt_texture.jpg",NULL);

	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the model object.", L"Error", MB_OK);
	}

	result = model->LoadModelFromFBX("assets/models/Grunt/Grunt.fbx");
	if (!result){
		MessageBox(getMainWnd(), L"Could not load in the FBX object.", L"Error", MB_OK);
	}
	gameObjectList.push_back(model);
	model->pos = Vector3f(0,1.5f,0);
	/*model2 = new ModelObject(*model);
	model2->pos = Vector3f(8,7,0);
	gameObjectList.push_back(model2);*/
}

void MainApp::initGrids(){
	//initialize the terrain
	bool result;

	terrain = new Grid();
	result = terrain->InitializeWithMultiTexture(md3dDevice,L"assets/defaultspec.dds", NULL,L"assets/stone2.dds",
																						 L"assets/ground0.dds",
																						 L"assets/grass0.dds");
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the terrain object.", L"Error", MB_OK);
	}
	result = terrain->GenerateGridFromTGA("assets/heightmap.tga");
	if(!result){
		MessageBox(getMainWnd(), L"Could properly generate heightmap.", L"Error", MB_OK);
	}
	gameObjectList.push_back(terrain);

	//initialize the water
	water = new Grid();
	result = water->InitializeWithTexture(md3dDevice,L"assets/water.jpg",L"assets/waterAlpha.png");
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the water object.", L"Error", MB_OK);
	}
	result = water->GenerateGrid(512,512,8);
	if(!result){
		MessageBox(getMainWnd(), L"Could properly generate heightmap.", L"Error", MB_OK);
	}
	water->pos = Vector3f(0,1.1f,0);
	gameObjectList.push_back(water);
}

void MainApp::initShaders(){
	bool result;
	// Create the text shader object.
	texShader = new TexShader();
	// Initialize the tex shader object.
	result = texShader->Initialize(md3dDevice, getMainWnd(),REGULAR);
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the tex shader object.", L"Error", MB_OK);
	}

	shaderList.push_back(texShader);
	multiTexShader = new TexShader();
	// Initialize the multi-tex shader object.
	result = multiTexShader->Initialize(md3dDevice, getMainWnd(),MULTI);
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the multi tex shader object.", L"Error", MB_OK);
	}

	shaderList.push_back(multiTexShader);
}

///Any input key processing - to it here
void MainApp::processInput(float dt){
	if (GetAsyncKeyState(VK_ESCAPE)){
		PostQuitMessage(0);
	}

	//if we are using the god camera - use standart input to move it
	if (currentCam == godCamera){
		if (GetAsyncKeyState('W')){
			godCamera->moveBackForward += godCamera->camMoveFactor*dt;
		}
		if (GetAsyncKeyState('S')){
			godCamera->moveBackForward -= godCamera->camMoveFactor*dt;
		}
		if (GetAsyncKeyState('A')){
			godCamera->moveLeftRight -= godCamera->camMoveFactor*dt;
		}
		if (GetAsyncKeyState('D')){
			godCamera->moveLeftRight += godCamera->camMoveFactor*dt;
		}
	}
	//else if we are using the player camera switch input to player movement
	else{
		if (GetAsyncKeyState('W')){
			model->MoveFacing(10*dt);
			model->pos.y = terrain->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		if (GetAsyncKeyState('S')){
			model->MoveFacing(-10*dt);
			model->pos.y = terrain->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		if (GetAsyncKeyState('A')){
			model->MoveStrafe(-10*dt);
			model->pos.y = terrain->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		if (GetAsyncKeyState('D')){
			model->MoveStrafe(10*dt);
			model->pos.y = terrain->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		playerCamera->SetPosition(model->pos);
	}

	if (GetAsyncKeyState(VK_UP)){
		light[0].dir += Vector3f(0,0,0.005f);
	}
	if (GetAsyncKeyState(VK_DOWN)){
		light[0].dir += Vector3f(0,0,-0.005f);
	}

	if (mouseInput)
		MouseInput();

	if ((GetAsyncKeyState(VK_SHIFT) & 0x8000)){
		SwitchCameras();
	}

	if (GetAsyncKeyState('B')){
		swapRasterizers();
		Sleep(100);
	}
	
}

void MainApp::MouseInput(){
	POINT mousePoint; 

	GetCursorPos(&mousePoint);

	if (mousePoint.x != clickedPoint.x || mousePoint.y != clickedPoint.y){
		float yaw = (clickedPoint.x - mousePoint.x) * mTimer.getDeltaTime();
		float pitch = (clickedPoint.y - mousePoint.y) * mTimer.getDeltaTime(); 

		if (currentCam == godCamera){
			currentCam->MoveYawPitch(yaw,pitch);
		}
		else{
			//if right mouse button is clicked - move model with the mouse
			if (mouseRightB){
				model->theta.y -= yaw;	
			}
			currentCam->MoveYawPitch(yaw,pitch);			
		}

		clickedPoint = mousePoint;
	}	
}

void MainApp::mouseScroll(int amount){
	if (currentCam == godCamera){
		if (amount > 0)
			godCamera->ModifyCamMovement(0.5f);
		else{
			godCamera->ModifyCamMovement(-0.5f);
		}	
	}
	else{
		if (amount > 0)
			playerCamera->SetPivotPoint(playerCamera->GetPivotPoint() + Vector3f(0,0,0.3f));
		else{
			playerCamera->SetPivotPoint(playerCamera->GetPivotPoint() + Vector3f(0,0,-0.3f));
		}		
	}
}

void MainApp::onResize(){
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, aspectRatio*PI, aspect, 1.0f, 1000.0f);
}

void MainApp::updateScene(float dt){
	switch (gameMode){
	case SINGLE:
		break;
	case SERVER:
		server->Update(dt);
		break;
	case CLIENT:
		//send info data to server every 0.1 second
		client->Update(dt);
		break;
	}
	
	processInput(dt);
	D3DApp::updateScene(dt);
	animateLights();
	water->AnimateUV(dt);
	
}

void MainApp::drawScene(){

	D3DApp::drawScene();
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->RSSetState(mCurrentRasterizer);
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

	// Generate the view matrix based on the camera's position.
	currentCam->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	currentCam->GetViewMatrix(mView);
	
	drawModels();	

	drawGrids();
	
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}

void MainApp::drawGrids(){
	//draw the terrain
	terrain->Render(mWVP);
	multiTexShader->RenderMultiTexturing(md3dDevice,terrain->GetIndexCount(),terrain->objMatrix,mView,mProj,currentCam->GetPosition(),light[lightType],
																															 terrain->GetSpecularTexture(),
																															 NULL,
																															 terrain->GetDiffuseMap(0),
																															 terrain->GetDiffuseMap(1),
																															 terrain->GetDiffuseMap(2),
																															 terrain->GetMaxHeight(),
																															 lightType);
	//get some blending going on
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(transparentBS, blendFactors, 0xffffffff);
	//draw the water
	water->Render(mWVP);
	texShader->RenderTexturing(md3dDevice,water->GetIndexCount(),water->objMatrix,mView,mProj, water->GetTexMatrix(), currentCam->GetPosition(), light[lightType],water->GetDiffuseTexture(),water->GetSpecularTexture());

}

void MainApp::drawModels(){
	//Render the Model
	model->Render(mWVP);
	texShader->RenderTexturing(md3dDevice,model->GetIndexCount(),model->objMatrix,mView,mProj, model->GetTexMatrix(),currentCam->GetPosition(),light[lightType],model->GetDiffuseTexture(),model->GetSpecularTexture());

	/*model2->Render(mWVP);
	texShader->RenderTexturing(md3dDevice,model2->GetIndexCount(),model2->objMatrix,mView,mProj,camera->GetPosition(),light[lightType],model2->GetDiffuseTexture(),model2->GetSpecularTexture());*/

}

void MainApp::animateLights(){
	// Set the light type based on user input.
	if(GetAsyncKeyState('1') & 0x8000) lightType = L_PARALLEL;
	if(GetAsyncKeyState('2') & 0x8000) lightType = L_POINT;
	if(GetAsyncKeyState('3') & 0x8000) lightType = L_SPOT;
	
	switch (lightType){
	case L_POINT:
		// The point light circles the scene as a function of time,
		// staying 7 units above the land's or water's surface.
		light[1].pos.x = 50.0f*cosf( mTimer.getGameTime() );
		light[1].pos.z = 50.0f*sinf( mTimer.getGameTime() );
		light[1].pos.y = Max(terrain->GetHeight(light[1].pos.x, light[1].pos.z), 0.0f) + 7.0f;
		break;
	case L_SPOT:
		// The spotlight takes on the camera position and is aimed in the
		// same direction the camera is looking. In this way, it looks
		// like we are holding a flashlight.
		light[2].pos = currentCam->GetPosition();
		D3DXVec3Normalize(&light[2].dir, &(currentCam->GetLookAtTarget()-currentCam->GetPosition()));
		break;
	}	
}

void MainApp::SwitchCameras(){
	if (currentCam == godCamera)
		currentCam = playerCamera;
	else
		currentCam = godCamera;
}
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
#include <list>

using namespace std;

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	~MainApp();
	int scale;

	float rotAngle;

	void processInput();

	void initApp();
	void initCameras();
	void initModels();
	void initShaders();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	void mouseScroll(int amount);

	LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);
	
private:
	void buildFX();
	void buildVertexLayouts();
	void animateLights();
	void SwitchCameras();
	void MouseInput();
 
private:

	std::list<Shader*>		shaderList;
	std::list<GameObject*>	gameObjectList;
	std::list<GameCamera*>	gameCameraList;

	Light			light[3]; // 0 (parallel), 1 (point), 2 (spot)
	LIGHT_TYPE		lightType; // 0 (parallel), 1 (point), 2 (spot)

	ModelObject		*model;
	//ModelObject	*model2;
	Grid			*grid;

	GameCamera		*godCamera;
	GameCamera		*playerCamera;
	GameCamera		*currentCam;

	TexShader		*texShader;
	TexShader		*multiTexShader;
	LightShader		*lightShader;
	Shader			*colorShader;

	float			aspectRatio;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	bool			mouseInput;
};

LRESULT MainApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam){
	D3DApp::msgProc(msg,wParam,lParam);
	switch( msg ){
	// Get network messages
	case WM_SOCKET:
		
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
	
	theApp.initApp();

	

	return theApp.run();
}

MainApp::MainApp(HINSTANCE hInstance)
: D3DApp(hInstance)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 

	aspectRatio = 0.5f;
	rotAngle = 0.0f;

	mouseInput = false;

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
	grid = NULL;
	lightShader = NULL;	
	texShader = NULL;
	colorShader = NULL;
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
}

void MainApp::initApp(){
	D3DApp::initApp();		

	initCameras();
	initModels();
	initShaders();	
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
	grid = new Grid();
	result = grid->InitializeWithMultiTexture(md3dDevice,L"assets/defaultspec.dds", NULL,L"assets/stone2.dds",
																						 L"assets/ground0.dds",
																						 L"assets/grass0.dds");
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the grid object.", L"Error", MB_OK);
	}
	result = grid->GenerateGridFromTGA("assets/heightmap.tga");
	if(!result){
		MessageBox(getMainWnd(), L"Could properly generate heightmap.", L"Error", MB_OK);
	}
	gameObjectList.push_back(grid);

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
void MainApp::processInput(){
	if (GetAsyncKeyState(VK_ESCAPE)){
		PostQuitMessage(0);
	}

	//if we are using the god camera - use standart input to move it
	if (currentCam == godCamera){
		if (GetAsyncKeyState('W')){
			godCamera->moveBackForward += godCamera->camMoveFactor*mTimer.getDeltaTime();
		}
		if (GetAsyncKeyState('S')){
			godCamera->moveBackForward -= godCamera->camMoveFactor*mTimer.getDeltaTime();
		}
		if (GetAsyncKeyState('A')){
			godCamera->moveLeftRight -= godCamera->camMoveFactor*mTimer.getDeltaTime();
		}
		if (GetAsyncKeyState('D')){
			godCamera->moveLeftRight += godCamera->camMoveFactor*mTimer.getDeltaTime();
		}
	}
	//else if we are using the player camera switch input to player movement
	else{
		if (GetAsyncKeyState('W')){
			model->MoveFacing(10*mTimer.getDeltaTime());
			model->pos.y = grid->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		if (GetAsyncKeyState('S')){
			model->MoveFacing(-10*mTimer.getDeltaTime());
			model->pos.y = grid->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		if (GetAsyncKeyState('A')){
			model->MoveStrafe(-10*mTimer.getDeltaTime());
			model->pos.y = grid->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		if (GetAsyncKeyState('D')){
			model->MoveStrafe(10*mTimer.getDeltaTime());
			model->pos.y = grid->GetHeight(model->pos.x,model->pos.z) + 1.0f;
		}
		playerCamera->SetPosition(model->pos);
	}

	//enable or disable mouse input
	if ((GetAsyncKeyState(VK_RETURN) & 0x8000)){
		mouseInput = !mouseInput;
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
	POINT mousePos; 
	int mid_x = mClientWidth >> 1; //dividing by 2 in a fancy way
	int mid_y = mClientHeight >> 1; //dividing by 2 in a fancy way

	GetCursorPos(&mousePos);
	SetCursorPos(mid_x, mid_y);

	float yaw = (float)( (mid_x - mousePos.x) ) / 1000; 
	float pitch = (float)( (mid_y - mousePos.y) ) / 1000;

	if (currentCam == godCamera){
		currentCam->MoveYawPitch(yaw,pitch);
	}
	else{
		model->theta.y -= yaw;
		currentCam->SetRotation(model->theta);
		currentCam->MoveYawPitch(0.0f,pitch);
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
	D3DApp::updateScene(dt);
	animateLights();
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

	//Render the Model
	model->Render(mWVP);
	texShader->RenderTexturing(md3dDevice,model->GetIndexCount(),model->objMatrix,mView,mProj,currentCam->GetPosition(),light[lightType],model->GetDiffuseTexture(),model->GetSpecularTexture());

	/*model2->Render(mWVP);
	texShader->RenderTexturing(md3dDevice,model2->GetIndexCount(),model2->objMatrix,mView,mProj,camera->GetPosition(),light[lightType],model2->GetDiffuseTexture(),model2->GetSpecularTexture());*/

	grid->Render(mWVP);
	multiTexShader->RenderMultiTexturing(md3dDevice,grid->GetIndexCount(),grid->objMatrix,mView,mProj,currentCam->GetPosition(),light[lightType],
																															 grid->GetSpecularTexture(),
																															 NULL,
																															 grid->GetDiffuseMap(0),
																															 grid->GetDiffuseMap(1),
																															 grid->GetDiffuseMap(2),
																															 grid->GetMaxHeight(),
																															 lightType);


	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);
	mSwapChain->Present(0, 0);
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
		light[1].pos.y = Max(grid->GetHeight(light[1].pos.x, light[1].pos.z), 0.0f) + 7.0f;
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
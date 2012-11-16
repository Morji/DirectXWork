//=============================================================================
// MainApp.cpp based on Frank Lunas Colored Cube App
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

#include "d3dApp.h"
#include "GameObject.h"
#include "CubeObject.h"
#include "Shader.h"
#include "GameCamera.h"
#include "Light.h"
#include "LightShader.h"
#include "TexShader.h"
#include "Grid.h"

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	~MainApp();
	int scale;

	float rotAngle;

	void processInput();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	void mouseScroll(int amount);

private:
	void buildFX();
	void buildVertexLayouts();
	void animateLights();
 
private:

	Light			light[3]; // 0 (parallel), 1 (point), 2 (spot)
	LIGHT_TYPE		lightType; // 0 (parallel), 1 (point), 2 (spot)

	CubeObject		*cube;
	Grid			*grid;

	GameCamera		*camera;

	TexShader		*texShader;
	TexShader		*multiTexShader;
	LightShader		*lightShader;
	Shader			*colorShader;

	float			aspectRatio;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


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
	cube = NULL;
	grid = NULL;
	camera = NULL;
	lightShader = NULL;	
	texShader = NULL;
	colorShader = NULL;
}

MainApp::~MainApp(){
	if( md3dDevice )
		md3dDevice->ClearState();

	// Release the light shader object.
	if(lightShader){
		lightShader->Shutdown();
		delete lightShader;
		lightShader = NULL;
	}

	if (colorShader){
		colorShader->Shutdown();
		delete colorShader;
		colorShader = NULL;
	}

	if(texShader){
		texShader->Shutdown();
		delete texShader;
		texShader = NULL;
	}

	if (multiTexShader){
		multiTexShader->Shutdown();
		delete multiTexShader;
		multiTexShader = NULL;
	}

	// Release the model object.
	if(cube){
		cube->Shutdown();
		delete cube;
		cube = NULL;
	}

	if (grid){
		grid->Shutdown();
		delete grid;
		grid = NULL;
	}
	// Release the camera object.
	if(camera){
		delete camera;
		camera = NULL;
	}
}

void MainApp::initApp(){
	D3DApp::initApp();	
	bool result;

	// Create the camera object.
	camera = new GameCamera();

	// Set the initial position of the camera.
	camera->SetPosition(0.0f, 0.0f, -10.0f);

	cube = new CubeObject();

	grid = new Grid();

	result = grid->InitializeWithMultiTexture(md3dDevice,L"assets/defaultspec.dds", L"assets/blendmap.jpg",L"assets/stone2.dds",
																										   L"assets/ground0.dds",
																										   L"assets/grass0.dds");

	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the grid object.", L"Error", MB_OK);
	}

	grid->GenerateGridFromTGA("assets/heightmap.tga");

	// Create the text shader object.
	texShader = new TexShader();
	// Initialize the tex shader object.
	result = texShader->Initialize(md3dDevice, getMainWnd(),REGULAR);
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the tex shader object.", L"Error", MB_OK);
	}

	multiTexShader = new TexShader();
	// Initialize the multi-tex shader object.
	result = multiTexShader->Initialize(md3dDevice, getMainWnd(),MULTI);
	if(!result){
		MessageBox(getMainWnd(), L"Could not initialize the multi tex shader object.", L"Error", MB_OK);
	}
}

///Any input key processing - to it here
void MainApp::processInput(){

	camera->MouseMove(mClientWidth,mClientHeight);

	if (GetAsyncKeyState(VK_ESCAPE)){
		PostQuitMessage(0);
	}

	if (GetAsyncKeyState('W')){
		camera->moveBackForward += camera->camMoveFactor*mTimer.getDeltaTime();
	}
	if (GetAsyncKeyState('S')){
		camera->moveBackForward -= camera->camMoveFactor*mTimer.getDeltaTime();
	}
	if (GetAsyncKeyState('A')){
		camera->moveLeftRight -= camera->camMoveFactor*mTimer.getDeltaTime();
	}
	if (GetAsyncKeyState('D')){
		camera->moveLeftRight += camera->camMoveFactor*mTimer.getDeltaTime();
	}

	if (GetAsyncKeyState('F')){
		cube->theta += D3DXVECTOR3(0,1.5f,0)*mTimer.getDeltaTime();
	}

	if (GetAsyncKeyState('B')){
		swapRasterizers();
		Sleep(100);
	}
}

void MainApp::mouseScroll(int amount){
	if (amount > 0)
		camera->ModifyCamMovement(0.5f);
	else{
		camera->ModifyCamMovement(-0.5f);
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
	camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	camera->GetViewMatrix(mView);

	grid->Render(mWVP);
	multiTexShader->RenderMultiTexturing(md3dDevice,grid->GetIndexCount(),grid->objMatrix,mView,mProj,camera->GetPosition(),light[lightType],
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
		light[2].pos = camera->GetPosition();
		D3DXVec3Normalize(&light[2].dir, &(camera->GetLookAtTarget()-camera->GetPosition()));
		break;
	}	
}
//=============================================================================
// Triangle App.cpp based on Frank Lunas Colored Cube App
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

#include "d3dApp.h"
#include "Vertex.h"
#include "GameObject.h"


class ColoredTriangleApp : public D3DApp
{
public:
	ColoredTriangleApp(HINSTANCE hInstance);
	~ColoredTriangleApp();
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	int scale;

	float rotAngle;

	void processInput();

	void DrawTriangle(D3D10_TECHNIQUE_DESC techDesc);

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
	void buildRasterizer();
 
private:

	GameObject obj1,obj2;
	
	float aspectRatio;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10RasterizerState* pRasterizer;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	float mTheta;
	float mPhi;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredTriangleApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

ColoredTriangleApp::ColoredTriangleApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 

	aspectRatio = 0.25f;
	rotAngle = 0.0f;

	obj1.pos = Vector3f(0,0,0);
	obj2.pos = Vector3f(-4,0,0);
	
}

ColoredTriangleApp::~ColoredTriangleApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredTriangleApp::initApp()
{
	D3DApp::initApp();	
		
	DWORD mNumVertices = 3;
	DWORD  mNumFaces    = 1; // 2 per quad
	scale=2.0f;

	// Create vertex buffer
    Vertex vertices[] =
    {
		{D3DXVECTOR3(-1.0f, -1.0f, 0.0f)},
		{D3DXVECTOR3(-1.0f, +1.0f, 0.0f)},
		{D3DXVECTOR3(+1.0f, +1.0f, 0.0f)},		
    };

	// Scale the geometry.
	/*for(DWORD i = 0; i < mNumVertices; ++i){
		vertices[i].pos *= scale;
	}*/	

	obj1.init(md3dDevice,mNumVertices,mNumFaces);
	obj1.scale = D3DXVECTOR3(scale,scale,scale);
	obj1.theta = D3DXVECTOR3(0,0.005f,0);
	//declare vertex buffer
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	buildRasterizer();
	buildFX();	
	buildVertexLayouts();
	
}

void ColoredTriangleApp::processInput(){
	if (keys[VK_ESCAPE]){
		PostQuitMessage(0);
		keys[VK_ESCAPE] = false;
	}
	if (keys[VK_UP]){
		aspectRatio += 0.005f;

		float aspect = (float)mClientWidth/mClientHeight;
		D3DXMatrixPerspectiveFovLH(&mProj, aspectRatio*PI, aspect, 1.0f, 1000.0f);

		keys[VK_UP] = false;
	}
	if (keys[VK_DOWN]){
		aspectRatio -= 0.005f;

		float aspect = (float)mClientWidth/mClientHeight;
		D3DXMatrixPerspectiveFovLH(&mProj, aspectRatio*PI, aspect, 1.0f, 1000.0f);

		keys[VK_DOWN] = false;
	}
}

void ColoredTriangleApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, aspectRatio*PI, aspect, 1.0f, 1000.0f);
}

void ColoredTriangleApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Build the view matrix.
	D3DXVECTOR3 pos(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
	
}

void ColoredTriangleApp::drawScene()
{
	D3DApp::drawScene();
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->RSSetState(pRasterizer);
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDevice->IASetInputLayout(mVertexLayout);
    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   

	// set constants
	mWVP = mView*mProj;
	
	D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );

	obj1.theta += D3DXVECTOR3(0,0.001f,0);
	obj1.setTrans(mWVP);	

	mfxWVPVar->SetMatrix((float*)&obj1.objMatrix);
    DrawTriangle(techDesc);

	/*D3DXMatrixTranslation(&obj2.worldMatrix,obj2.pos.x,obj2.pos.y,obj2.pos.z);	

	D3DXMatrixRotationY(&obj2.worldMatrix,rotAngle);

	rotAngle+= 0.001f;

	obj2.worldMatrix = obj2.worldMatrix*mWVP;

	mfxWVPVar->SetMatrix((float*)&obj2.worldMatrix);
	DrawTriangle(techDesc);*/

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);
	mSwapChain->Present(0, 0);
}

void ColoredTriangleApp::DrawTriangle(D3D10_TECHNIQUE_DESC techDesc){

	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mTech->GetPassByIndex( p )->Apply(0);        
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->Draw(3, 0);
    }
}

void ColoredTriangleApp::buildRasterizer(){

	D3D10_RASTERIZER_DESC rasterizerState;
	rasterizerState.CullMode = D3D10_CULL_NONE;
	rasterizerState.FillMode = D3D10_FILL_WIREFRAME;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	HRESULT result = md3dDevice->CreateRasterizerState( &rasterizerState, &pRasterizer);
	md3dDevice->RSSetState(pRasterizer);

}

void ColoredTriangleApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();

}

void ColoredTriangleApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		//{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 1, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 
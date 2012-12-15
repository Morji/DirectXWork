//=======================================================================================
// d3dApp.h by Frank Luna (C) 2008 All Rights Reserved.
//
// Simple Direct3D demo application class.  
// Make sure you link: d3d10.lib d3dx10d.lib dxerr.lib dxguid.lib.
// Link d3dx10.lib for release mode builds instead of d3dx10d.lib.
//=======================================================================================


#ifndef D3DAPP_H
#define D3DAPP_H


#include "d3dUtil.h"
#include "GameTimer.h"
#include <string>

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE getAppInst();
	HWND      getMainWnd();

	int run();

	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.

	virtual void initApp();
	virtual void onResize();// reset projection/etc
	virtual void updateScene(float dt);
	virtual void drawScene(); 
	virtual void processInput(void){};
	virtual void mouseScroll(int amount);
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	void swapRasterizers();

private:
	long stop;
	long remaining;

protected:
	void initMainWindow();
	void buildRasterizers();
	void buildBlendStates();
	void buildDepthStencilStates();
	void initDirect3D();
	
protected:

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;

	D3D10_VIEWPORT vp;
	GameTimer mTimer;

	std::wstring mFrameStats;
 
	ID3D10Device*				md3dDevice;

	IDXGISwapChain*				mSwapChain;

	ID3D10Texture2D*			mDepthStencilBuffer;

	ID3D10RenderTargetView*		mRenderTargetView;

	ID3D10BlendState*			mTransparentBS;
	ID3D10BlendState*			mDrawReflectionBS;

	ID3D10DepthStencilView*		mDepthStencilView;

	ID3D10DepthStencilState*	mDrawMirrorDSS;
	ID3D10DepthStencilState*	mDrawReflectionDSS;

	ID3D10RasterizerState*		mCurrentRasterizer;
	ID3D10RasterizerState*		mRasterizerSolid;
	ID3D10RasterizerState*		mDynamicRasterizer;
	ID3D10RasterizerState*		mRasterizerWireframe;
	ID3D10RasterizerState*		mRasterizerCullCWRS;

	ID3DX10Font* mFont;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring		mMainWndCaption;
	D3D10_DRIVER_TYPE	md3dDriverType;
	D3DXCOLOR			mClearColor;
	int					mClientWidth;
	int					mClientHeight;

	POINT				clickedPoint;
	bool				mouseInput;
	bool				mouseRightB;	//wether its the right mouse button that is clicked
};




#endif // D3DAPP_H
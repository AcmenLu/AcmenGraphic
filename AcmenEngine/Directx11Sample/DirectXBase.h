#ifndef _DIRECTXBASE_H
#define _DIRECTXBASE_H
#include "MainWindows.h"

class DirectXBase
{
protected:
	ID3D11Device*			mD3D11Device;
	ID3D11DeviceContext*	mD3D11DeviceContext;
	IDXGISwapChain*			mSwapChain;
	ID3D11RenderTargetView*	mRenderTargetView;
	ID3D11Texture2D*		mDepthStencilBuffer;
	ID3D11DepthStencilView*	mDepthStencilView;
	MainWindows*			mWindows;

public:
	DirectXBase( ):
		mD3D11Device( 0 ),
		mD3D11DeviceContext( 0 ),
		mSwapChain( 0 ),
		mRenderTargetView( 0 ),
		mDepthStencilBuffer( 0 ),
		mDepthStencilView( 0 ),
		mWindows( 0 ) { }

	DirectXBase( MainWindows* windows ):
		mD3D11Device( 0 ),
		mD3D11DeviceContext( 0 ),
		mSwapChain( 0 ),
		mRenderTargetView( 0 ),
		mDepthStencilBuffer( 0 ),
		mDepthStencilView( 0 ),
		mWindows( windows ) { }

	~DirectXBase( );
	bool InitDirectX3D( );
	void OnResize( );
	void OnRender( ) { }
	bool SetUp( ){ return true; }

};

#endif
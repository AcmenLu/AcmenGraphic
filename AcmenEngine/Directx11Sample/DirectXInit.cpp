#define _DIRECTXINIT_H
#ifndef _DIRECTXINIT_H
#define _DIRECTXINIT_H

#include "DirectXBase.h"

class DirectXInit: public DirectXBase
{
public:
	DirectXInit( ){ }
	~DirectXInit( ){ };
	void OnRender( );
	bool SetUp( );
};

MainWindows* window = 0;
DirectXInit* directx = 0;

void OnIdle( float elapse )
{
	directx->OnRender( );
}

void DirectXInit::OnRender( )
{
	assert( mD3D11DeviceContext );
	assert( mSwapChain );
	XMVECTORF32 color = { 0.f, 1.f, 0.f, 1.6f };
	mD3D11DeviceContext->ClearRenderTargetView( mRenderTargetView, reinterpret_cast<const float*>( &color ) );
	mD3D11DeviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0 );
	mSwapChain->Present( 0, 0 );
}

bool DirectXInit::SetUp( )
{
	return true;
}

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	window = new MainWindows( hInstance, "AcmenWindow", 800, 600 );
	window->InitMainWindow( );
	directx = new DirectXInit( );
	directx->SetRenderWindows( window );
	directx->InitDirectX3D( );
	window->SetOnIdleCallBack( OnIdle );
	window->MessageLoop( );
	SafeDelete( window );
	SafeDelete( directx );
	return 0;
}

#endif
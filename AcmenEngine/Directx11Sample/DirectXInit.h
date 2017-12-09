#include "DirectXBase.h"

class DirectXInit: public DirectXBase
{
public:
	DirectXInit( ){ }
	DirectXInit( MainWindows* windows ):DirectXBase( windows ) { }
	~DirectXInit( ){ }
	void OnRender( );
	bool SetUp( ){ return true; }
	
	/*{
		assert( mD3D11DeviceContext );
		assert( mSwapChain );
		XMVECTORF32 color = { 0.f, 1.f, 0.f, 1.6f };
		mD3D11DeviceContext->ClearRenderTargetView( mRenderTargetView, reinterpret_cast<const float*>( &color ) );
		mD3D11DeviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0 );
		mSwapChain->Present( 0, 0 );
	}*/
};
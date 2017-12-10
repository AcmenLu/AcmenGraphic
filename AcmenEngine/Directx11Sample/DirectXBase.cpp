#include "DirectXBase.h"

DirectXBase::~DirectXBase( )
{
	ReleaseCOM( mD3D11Device );
	ReleaseCOM( mD3D11DeviceContext );
	ReleaseCOM( mSwapChain );
	ReleaseCOM(  mRenderTargetView );
	ReleaseCOM( mDepthStencilBuffer );
	ReleaseCOM(  mDepthStencilView );
}

bool DirectXBase::InitDirectX3D( )
{
	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE };
	UINT driverTypesCount = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] = {  
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0 };
	UINT featureLevelsCount = ARRAYSIZE( featureLevels );

	// ¹¹ÔìDXGI_SWAP_CHAIN_DESC
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width = mWindows->GetWindowsWidth( );
	swapChainDesc.BufferDesc.Height = mWindows->GetWindowsHeight( );
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Windowed = true;
	swapChainDesc.OutputWindow = mWindows->GetWindows( );
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	
	UINT createDeviceFlag = 0;
	#if defined( DEBUG ) || defined( _DEBUG )
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL pfeatureLevel;
	HRESULT hr;
	for ( UINT index = 0; index < driverTypesCount; ++ index )
	{
		D3D_DRIVER_TYPE driveType = driverTypes[index];
		hr = D3D11CreateDeviceAndSwapChain( NULL, driveType, NULL, createDeviceFlag, featureLevels,
			featureLevelsCount, D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mD3D11Device, &pfeatureLevel, &mD3D11DeviceContext );

		if ( SUCCEEDED( hr ) )
			break;
	}
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create device and swap chain failed!", 0, 0 );
		return false;
	}
	return true;
}

void DirectXBase::OnResize( )
{
	assert( mD3D11DeviceContext );
	assert( mD3D11Device );
	assert( mSwapChain );

	ReleaseCOM( mRenderTargetView );
	ReleaseCOM( mDepthStencilView );
	ReleaseCOM( mDepthStencilBuffer );

	int width = mWindows->GetWindowsWidth( );
	int height = mWindows->GetWindowsHeight( );

	ID3D11Texture2D* pBackBuffer = 0;
	HRESULT hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Get swap chain back buffer failed!", 0, 0 );
		return;
	}
	hr = mD3D11Device->CreateRenderTargetView( pBackBuffer, 0, &mRenderTargetView );
	ReleaseCOM( pBackBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create render target view failed!", 0, 0 );
		return;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = mD3D11Device->CreateTexture2D( &textureDesc, 0, &mDepthStencilBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create depth and stencil buffer failed!", 0, 0 );
		return;
	}
	hr = mD3D11Device->CreateDepthStencilView( mDepthStencilBuffer, 0, &mDepthStencilView );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create depth and stencil view failed!", 0, 0 );
		return;
	}
	mD3D11DeviceContext->OMSetRenderTargets( 1, &mRenderTargetView, mDepthStencilView );

	D3D11_VIEWPORT viewPort;  
	viewPort.Width = static_cast<FLOAT>( width );  
	viewPort.Height = static_cast<FLOAT>( height );  
	viewPort.MaxDepth = 1.f;
	viewPort.MinDepth = 0.f;
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	mD3D11DeviceContext->RSSetViewports( 1,&viewPort );
}
#define _DIRECTXBOX_H
#ifndef _DIRECTXBOX_H
#define _DIRECTXBOX_H

#include "DirectXBase.h"
#define PI 3.1415926535f

struct Vertex  
{  
    XMFLOAT3    pos;  
    XMFLOAT4    color;  
};  

class DirectXBox: public DirectXBase
{
private:
	ID3D11Buffer*					mVertexBuffer;
	ID3D11Buffer*					mIndexBuffer;
	ID3D11InputLayout*				mInputLayout;
	ID3DX11Effect*					mD3DEffect;
	ID3DX11EffectTechnique*			mTechnique;
	ID3DX11EffectMatrixVariable*	mFxWorldViewProj;

	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjMatrix;

	float mTheta;
	float mPhi;
	float mRadius;

public:
	DirectXBox( ): mVertexBuffer( 0 ),
		mIndexBuffer( 0 ), mInputLayout( 0 ), mD3DEffect( 0 ),
		mTechnique( 0 ), mFxWorldViewProj( 0 ), mTheta( 0.0f ),
		mPhi( 0.0f ), mRadius( 5.0f )
	{
		XMMATRIX I = XMMatrixIdentity( );
		XMStoreFloat4x4( &mWorldMatrix, I );
		XMStoreFloat4x4( &mViewMatrix, I );
		XMStoreFloat4x4( &mProjMatrix, I );
	}

	~DirectXBox( )
	{
		ReleaseCOM( mVertexBuffer );
		ReleaseCOM( mIndexBuffer );
		ReleaseCOM( mInputLayout );
	}

	bool BuildGeometryBuffers( );
	bool BuildShader( );
	bool CreateInputLayout( );

	bool InitDirectX3D( );
	bool OnResize( );
	void OnUpdate( float elapse );
	void OnRender( );
	bool SetUp( );
};

bool DirectXBox::BuildGeometryBuffers( )
{
	// Create vertex buffer
    Vertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) }
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof( Vertex ) * 8;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
	HRESULT hr = mD3D11Device->CreateBuffer( &vbd, &vinitData, &mVertexBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create vertex buffer failed!", 0, 0 );
		return false;
	}
    
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof( UINT ) * 36;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
	hr = mD3D11Device->CreateBuffer( &ibd, &iinitData, &mIndexBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create vertex buffer failed!", 0, 0 );
		return false;
	}
	return true;
}

bool DirectXBox::BuildShader( )
{
	DWORD shaderFlags = 0;

	#if defined( DEBUG ) || defined( _DEBUG )

    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	
	#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile( "color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 
		0, 0, &compiledShader, &compilationMsgs, 0 );

	// compilationMsgs can store errors or warnings.
	if( compilationMsgs != 0 || FAILED( hr ) )
	{
		MessageBox( 0, (char*)compilationMsgs->GetBufferPointer( ), 0, 0 );
		ReleaseCOM(compilationMsgs);
		return false;
	}

	hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize( ), 
		0, mD3D11Device, &mD3DEffect );
	ReleaseCOM( compiledShader );
	if( FAILED( hr ) )
	{
		MessageBox( 0, "Create effect failed", 0, 0 );
		return false;
	}

	mTechnique = mD3DEffect->GetTechniqueByName( "ColorTech" );
	mFxWorldViewProj = mD3DEffect->GetVariableByName( "gWorldViewProj" )->AsMatrix( );
	return true;
}

bool DirectXBox::CreateInputLayout( )
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    mTechnique->GetPassByIndex( 0 )->GetDesc( &passDesc );
	HRESULT hr = mD3D11Device->CreateInputLayout( vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout );
	if ( FAILED( hr ) )
		return false;

	return true;
}

bool DirectXBox::InitDirectX3D( )
{
	DirectXBase::InitDirectX3D( );
	return OnResize( );
}

bool DirectXBox::OnResize( )
{
	DirectXBase::OnResize( );
	XMMATRIX P = XMMatrixPerspectiveFovLH( 0.25f * PI, mWindows->AspectRatio( ), 1.0f, 1000.0f );
	XMStoreFloat4x4( &mProjMatrix, P );
	return true;
}

void DirectXBox::OnUpdate( float elapse )
{
	XMVECTOR pos    = XMVectorSet( mRadius * sinf( elapse ), mRadius, mRadius, 1.0f );
	XMVECTOR target = XMVectorZero( );
	XMVECTOR up     = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	XMMATRIX V = XMMatrixLookAtLH( pos, target, up );
	XMStoreFloat4x4( &mViewMatrix, V );
}

void DirectXBox::OnRender( )
{
	assert( mD3D11DeviceContext );
	assert( mSwapChain );
	XMVECTORF32 color = { 0.f, 0.0f, 0.f, 1.0f };
	mD3D11DeviceContext->ClearRenderTargetView( mRenderTargetView, reinterpret_cast<const float*>( &color ) );
	mD3D11DeviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0 );

	mD3D11DeviceContext->IASetInputLayout( mInputLayout );
    mD3D11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof( Vertex );
    UINT offset = 0;
	mD3D11DeviceContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );
	mD3D11DeviceContext->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set constants
	XMMATRIX world = XMLoadFloat4x4( &mWorldMatrix );
	XMMATRIX view  = XMLoadFloat4x4( &mViewMatrix );
	XMMATRIX proj  = XMLoadFloat4x4( &mProjMatrix );
	XMMATRIX worldViewProj = world * view * proj;

	mFxWorldViewProj->SetMatrix( reinterpret_cast<float*>( &worldViewProj ) );

    D3DX11_TECHNIQUE_DESC techDesc;
    mTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
		mTechnique->GetPassByIndex( p )->Apply( 0, mD3D11DeviceContext );
		mD3D11DeviceContext->DrawIndexed( 36, 0, 0 );
    }

	mSwapChain->Present( 0, 0 );
}

bool DirectXBox::SetUp( )
{
	if ( BuildGeometryBuffers( ) && BuildShader( ) &&  CreateInputLayout( ) )
		return true;

	return false;
}

// --------------------------------------------------------------------------------
// --------------------------------Main Èë¿Ú---------------------------------------
//---------------------------------------------------------------------------------
MainWindows* window = 0;
DirectXBox* directx = 0;

void OnIdle( float elapse )
{
	directx->OnUpdate( elapse );
	directx->OnRender( );
}

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	window = new MainWindows( hInstance, "AcmenWindow", 800, 600 );
	window->InitMainWindow( );
	directx = new DirectXBox( );
	directx->SetRenderWindows( window );
	if ( !directx->InitDirectX3D( ) )
	{
		SafeDelete( window );
		SafeDelete( directx );
		return -1;
	}
	if ( !directx->SetUp( ) )
	{
		SafeDelete( window );
		SafeDelete( directx );
		return -1;
	}

	window->SetOnIdleCallBack( OnIdle );
	window->MessageLoop( );
	SafeDelete( window );
	SafeDelete( directx );
	return 0;
}

#endif
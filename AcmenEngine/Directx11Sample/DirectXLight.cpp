//#define _DIRECTXLIGHT_H
#ifndef _DIRECTXLIGHT_H
#define _DIRECTXLIGHT_H

#include "DirectXBase.h"
#include "GeometryGenerator.h"
#include "LightHelper.h"
#include <vector>

#define PI 3.1415926535f

struct Vertex  
{  
    XMFLOAT3    position;  
    XMFLOAT3    normal;  
};  

class DirectXLight: public DirectXBase
{
private:
	ID3D11Buffer*					mVertexBuffer;
	ID3D11Buffer*					mIndexBuffer;
	ID3D11InputLayout*				mInputLayout;
	ID3DX11Effect*					mD3DEffect;
	ID3DX11EffectTechnique*			mTechnique;

	ID3DX11EffectVariable*			mFxDirectionLight;
	ID3DX11EffectVariable*			mFxPointLight;
	ID3DX11EffectVariable*			mFxSpotLight;
	ID3DX11EffectVectorVariable*	mFxEyePosition;

	ID3DX11EffectMatrixVariable*	mFxWorld;
	ID3DX11EffectMatrixVariable*	mFxWorldInvTranspose;
	ID3DX11EffectMatrixVariable*	mFxWorldViewProj;
	ID3DX11EffectVariable*			mFxMaterial;

	GeometryGenerator::MeshData		mMeshData;
	DirectionalLight				mDirectionalLight;
	PointLight						mPointLight;
	SpotLight						mSpotLight;

	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjMatrix;

	float mTheta;
	float mPhi;
	float mRadius;

public:
	DirectXLight( ): mVertexBuffer( 0 ),
		mIndexBuffer( 0 ), mInputLayout( 0 ), mD3DEffect( 0 ),
		mTechnique( 0 ), mFxDirectionLight( 0 ), mFxPointLight( 0 ),
		mFxSpotLight( 0 ),mFxEyePosition( 0 ), mFxWorld( 0 ), 
		mFxWorldInvTranspose( 0 ), mFxWorldViewProj( 0 ), mFxMaterial( 0 ),
		mTheta( 0.0f ), mPhi( 0.0f ), mRadius( 5.0f )
	{
		XMMATRIX I = XMMatrixIdentity( );
		XMStoreFloat4x4( &mWorldMatrix, I );
		XMStoreFloat4x4( &mViewMatrix, I );
		XMStoreFloat4x4( &mProjMatrix, I );
	}

	~DirectXLight( )
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

bool DirectXLight::BuildGeometryBuffers( )
{
	GeometryGenerator geoGen;
	geoGen.CreateBox( 3.0f, 3.0f, 3.0f, mMeshData );

	std::vector<Vertex> vertices( mMeshData.Vertices.size( ) );
	for ( UINT i = 0; i < vertices.size( ); i ++ )
	{
		vertices[i].position = mMeshData.Vertices[i].Position;
		vertices[i].normal = mMeshData.Vertices[i].Normal;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof( Vertex ) * 8;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
	HRESULT hr = mD3D11Device->CreateBuffer( &vbd, &vinitData, &mVertexBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create vertex buffer failed!", 0, 0 );
		return false;
	}
    
	std::vector<UINT> indices( mMeshData.Indices.size( ) );
	for ( UINT i = 0; i < indices.size( ); i ++ )
	{
		indices[i] = mMeshData.Indices[i];
	}

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof( UINT ) * 36;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
	hr = mD3D11Device->CreateBuffer( &ibd, &iinitData, &mIndexBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, "Create vertex buffer failed!", 0, 0 );
		return false;
	}
	return true;
}

bool DirectXLight::BuildShader( )
{
	DWORD shaderFlags = 0;

	#if defined( DEBUG ) || defined( _DEBUG )

    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	
	#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile( "FX/Lighting.fx", 0, 0, 0, "fx_5_0", shaderFlags, 
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

	mTechnique				= mD3DEffect->GetTechniqueByName( "ColorTech" );
	mFxDirectionLight		= mD3DEffect->GetVariableByName( "gDirLight" );
	mFxPointLight			= mD3DEffect->GetVariableByName( "gPointLight" );
	mFxSpotLight			= mD3DEffect->GetVariableByName( "gSpotLight" );
	mFxEyePosition			= mD3DEffect->GetVariableByName( "gEyePosW" )->AsVector( );

	mFxWorld				= mD3DEffect->GetVariableByName( "gWorld" )->AsMatrix( );
	mFxWorldInvTranspose	= mD3DEffect->GetVariableByName( "gWorldViewProj" )->AsMatrix( );
	mFxWorldViewProj		= mD3DEffect->GetVariableByName( "gWorldViewProj" )->AsMatrix( );
	mFxMaterial				= mD3DEffect->GetVariableByName( "gMaterial" );
	return true;
}

bool DirectXLight::CreateInputLayout( )
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

bool DirectXLight::InitDirectX3D( )
{
	DirectXBase::InitDirectX3D( );
	return OnResize( );
}

bool DirectXLight::OnResize( )
{
	DirectXBase::OnResize( );
	XMMATRIX P = XMMatrixPerspectiveFovLH( 0.25f * PI, mWindows->AspectRatio( ), 1.0f, 1000.0f );
	XMStoreFloat4x4( &mProjMatrix, P );
	return true;
}

void DirectXLight::OnUpdate( float elapse )
{
	XMVECTOR pos    = XMVectorSet( mRadius * sinf( elapse ), mRadius, mRadius, 1.0f );
	XMVECTOR target = XMVectorZero( );
	XMVECTOR up     = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	XMMATRIX V = XMMatrixLookAtLH( pos, target, up );
	XMStoreFloat4x4( &mViewMatrix, V );
}

void DirectXLight::OnRender( )
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

bool DirectXLight::SetUp( )
{
	if ( BuildGeometryBuffers( ) && BuildShader( ) &&  CreateInputLayout( ) )
		return true;

	return false;
}

// --------------------------------------------------------------------------------
// --------------------------------Main Èë¿Ú---------------------------------------
//---------------------------------------------------------------------------------
MainWindows* window = 0;
DirectXLight* directx = 0;

void OnIdle( float elapse )
{
	directx->OnUpdate( elapse );
	directx->OnRender( );
}

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	window = new MainWindows( hInstance, "AcmenWindow", 800, 600 );
	window->InitMainWindow( );
	directx = new DirectXLight( );
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
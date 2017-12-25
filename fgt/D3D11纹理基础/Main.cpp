#include <WinApp.h>
#include <AppUtil.h>
#include <Lights.h>
#include <GeometryGens.h>
#include <d3dx11effect.h>
#include <D3DX11async.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

//定义顶点结构：
// 1. 位置
// 2. 法线
// 3. 纹理坐标
struct Vertex
{
	XMFLOAT3	pos;
	XMFLOAT3	normal;
	XMFLOAT2	tex;
};

class TextureDemo: public WinApp
{
public:
	TextureDemo(HINSTANCE hInst, std::wstring title = L"D3D11学习 纹理使用", int width = 640, int height = 480);
	~TextureDemo();

	bool Init();
	bool Update(float delta);
	bool Render();

	//鼠标控制
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	bool BuildFX();
	bool BuildInputLayout();
	bool BuildBuffers();
	bool BuildTexture();

private:
	ID3D11InputLayout	*m_inputLayout;

	ID3D11Buffer		*m_VB;
	ID3D11Buffer		*m_IB;

	ID3DX11Effect		*m_fx;
	//针对Effect中的全局变量，在C++中的一对一相应的变量，用于修改Effect变量
	ID3DX11EffectShaderResourceVariable	*m_fxTex;
	ID3DX11EffectMatrixVariable			*m_fxWorldViewProj;
	ID3DX11EffectMatrixVariable			*m_fxWorldInvTranspose;
	ID3DX11EffectMatrixVariable			*m_fxWorld;
	ID3DX11EffectVariable				*m_fxSpotLight;
	ID3DX11EffectVariable				*m_fxMaterial;
	ID3DX11EffectVariable				*m_fxEyePos;

	ID3D11ShaderResourceView	*m_texView;
	GeoGen::MeshData	m_box;
	Lights::SpotLight	m_spotLight;
	Lights::Material	m_material;
	XMFLOAT3			m_eyePos;

	//是否使用光照（聚光灯）
	bool	m_useLight;

	//鼠标控制参数
	float	m_theta, m_phy;
	float	m_radius;
	POINT	m_lastPos;
};

TextureDemo::TextureDemo(HINSTANCE hInst, std::wstring title, int width, int height):WinApp(hInst,title,width,height),
	m_inputLayout(NULL),
	m_VB(NULL),
	m_IB(NULL),
	m_fx(NULL),
	m_fxTex(NULL),
	m_fxWorldViewProj(NULL),
	m_texView(NULL),
	m_useLight(false),
	m_theta(XM_PI*1.5f),
	m_phy(XM_PI*0.5f),
	m_radius(6.f)
{
	m_spotLight.ambient = XMFLOAT4(0.f,0.f,0.f,1.f);
	m_spotLight.diffuse = XMFLOAT4(0.5f,0.5f,0.5f,1.f);
	m_spotLight.specular = XMFLOAT4(0.3f,0.3f,0.3f,1.f);
	m_spotLight.pos = XMFLOAT3(m_radius*sin(m_phy)*cos(m_theta),m_radius*cos(m_phy),m_radius*sin(m_phy)*sin(m_theta));
	m_spotLight.range = 100.f;
	m_spotLight.theta = XMConvertToRadians(30.f);
	XMStoreFloat3(&m_spotLight.dir,
		XMVector3Normalize(XMVectorSet(-m_radius*sin(m_phy)*cos(m_theta),
									-m_radius*cos(m_phy),
									-m_radius*sin(m_phy)*sin(m_theta),0.f)));
	m_spotLight.att = XMFLOAT3(0.2f,0.f,0.f);
	m_spotLight.spot =60.f;

	m_material.ambient = XMFLOAT4(0.f,0.f,0.f,1.f);
	m_material.diffuse = XMFLOAT4(0.6f,0.6f,0.6f,1.f);
	m_material.specular = XMFLOAT4(0.3f,0.3f,0.3f,60.f);
}

TextureDemo::~TextureDemo()
{
	SafeRelease(m_texView);
	SafeRelease(m_fx);
	SafeRelease(m_inputLayout);
	SafeRelease(m_IB);
	SafeRelease(m_VB);
}

bool TextureDemo::BuildFX()
{
	ifstream fxFile("FX/BasicTex.fxo",ios::binary);
	if(!fxFile)
	{
		return false;
	}

	fxFile.seekg(0,ifstream::end);
	UINT size = static_cast<UINT>(fxFile.tellg());
	fxFile.seekg(0,ifstream::beg);

	vector<char> shader(size);
	fxFile.read(&shader[0],size);

	fxFile.close();

	if(FAILED(D3DX11CreateEffectFromMemory(&shader[0],size,0,m_d3dDevice,&m_fx)))
	{
		MessageBox(NULL,L"CreateEffect failed!",L"Error",MB_OK);
		return false;
	}

	m_fxWorldViewProj = m_fx->GetVariableByName("g_worldViewProj")->AsMatrix();
	m_fxWorldInvTranspose = m_fx->GetVariableByName("g_worldInvTranspose")->AsMatrix();
	m_fxWorld = m_fx->GetVariableByName("g_world")->AsMatrix();
	m_fxTex = m_fx->GetVariableByName("g_tex")->AsShaderResource();
	m_fxEyePos = m_fx->GetVariableByName("g_eyePos");
	m_fxSpotLight = m_fx->GetVariableByName("g_spotLight");
	m_fxMaterial = m_fx->GetVariableByName("g_material");

	return true;
}

bool TextureDemo::BuildInputLayout()
{
	//针对顶点结构定义相应的InputLayout
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] = 
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",  0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,   0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	ID3DX11EffectTechnique *tech = m_fx->GetTechniqueByName("Tex");
	D3DX11_PASS_DESC pDesc;
	tech->GetPassByIndex(0)->GetDesc(&pDesc);

	if(FAILED(m_d3dDevice->CreateInputLayout(inputDesc,3,pDesc.pIAInputSignature,pDesc.IAInputSignatureSize,&m_inputLayout)))
	{
		MessageBox(NULL,L"Create Input Layout failed!",L"Error",MB_OK);
		return false;
	}

	return true;
}

bool TextureDemo::BuildBuffers()
{
	//在这里随意修改相渲染的几何物体
	//选择相应的函数即可，十分方便
	//立方体
	//GeoGen::CreateBox(3.f,3.f,3.f,m_box);
	//圆柱
	GeoGen::CreateCylinder(0.8f,0.8f,3.f,30,10,m_box);
	GeoGen::AddCylinderBottomCap(0.8f,0.8f,3.f,30,10,m_box);
	GeoGen::AddCylinderTopCap(0.8f,0.8f,3.f,30,10,m_box);
	//球
	//GeoGen::CreateSphere(1.5f,30,30,m_box);

	vector<Vertex> vertices(m_box.vertices.size());
	for(UINT i=0; i<vertices.size(); ++i)
	{
		vertices[i].pos = m_box.vertices[i].pos;
		vertices[i].normal = m_box.vertices[i].normal;
		vertices[i].tex = m_box.vertices[i].tex;
	}
	D3D11_BUFFER_DESC vbDesc = {0};
	vbDesc.ByteWidth = sizeof(Vertex) * vertices.size();
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = &vertices[0];
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;
	if(FAILED(m_d3dDevice->CreateBuffer(&vbDesc,&vData,&m_VB)))
	{
		MessageBox(NULL,L"Create vertex buffer failed!",L"Error",MB_OK);
		return false;
	}

	vector<UINT> indices(m_box.indices.size());
	for(UINT i=0; i<indices.size(); ++i)
	{
		indices[i] = m_box.indices[i];
	}
	D3D11_BUFFER_DESC ibDesc = {0};
	ibDesc.ByteWidth = sizeof(UINT) * indices.size();
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = &indices[0];
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;
	if(FAILED(m_d3dDevice->CreateBuffer(&ibDesc,&iData,&m_IB)))
	{
		MessageBox(NULL,L"Create index buffer failed!",L"Error",MB_OK);
		return false;
	}
	
	return true;
}

bool TextureDemo::Init()
{
	if(!WinApp::Init())
		return false;
	if(!BuildFX())
		return false;
	if(!BuildTexture())
		return false;
	if(!BuildInputLayout())
		return false;
	if(!BuildBuffers())
		return false;

	return true;
}

bool TextureDemo::Update(float delta)
{
	XMMATRIX world = XMMatrixIdentity();
	XMVECTOR lookAt = XMVectorSet(0.f,0.f,-0.f,1.f);
	XMVECTOR m_eyePos = XMVectorSet(m_radius*sin(m_phy)*cos(m_theta),m_radius*cos(m_phy),m_radius*sin(m_phy)*sin(m_theta),1.f);
	XMVECTOR up = XMVectorSet(0.f,1.f,0.f,0.f);
	XMMATRIX view = XMMatrixLookAtLH(m_eyePos,lookAt,up);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI*0.25f,1.f*m_clientWidth/m_clientHeight,1.f,1000.f);

	XMStoreFloat3(&m_spotLight.dir,XMVector3Normalize(
		XMVectorSet(-m_radius*sin(m_phy)*cos(m_theta),-m_radius*cos(m_phy),-m_radius*sin(m_phy)*sin(m_theta),0.f)));
	XMStoreFloat3(&m_spotLight.pos,m_eyePos);

	XMMATRIX worldViewProj = world * view * proj;
	XMVECTOR det = XMMatrixDeterminant(world);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det,world));
	//更新针对单个物体的Effect全局变量
	m_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	m_fxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
	m_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));

	//更新针对每一帧的相应的Effect全局变量
	m_fxTex->SetResource(m_texView);
	m_fxEyePos->SetRawValue(&m_eyePos,0,sizeof(m_eyePos));
	m_fxSpotLight->SetRawValue(&m_spotLight,0,sizeof(m_spotLight));
	m_fxMaterial->SetRawValue(&m_material,0,sizeof(m_material));

	//按‘1’关闭光照，‘2’打开
	if(KeyDown('1'))
		m_useLight = false;
	else if(KeyDown('2'))
		m_useLight = true;

	return true;
}

bool TextureDemo::Render()
{
	m_deviceContext->ClearDepthStencilView(m_depthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.f,0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView,reinterpret_cast<const float*>(&Colors::Silver));
	m_deviceContext->IASetInputLayout(m_inputLayout);
	
	UINT offset = 0;
	UINT stride = sizeof(Vertex);
	m_deviceContext->IASetVertexBuffers(0,1,&m_VB,&stride,&offset);
	m_deviceContext->IASetIndexBuffer(m_IB,DXGI_FORMAT_R32_UINT,0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//根据光照是否打开选择相应的"Technique"
	ostringstream techNameOss;
	techNameOss<<"Tex";
	if(m_useLight)
		techNameOss<<"Light";

	D3DX11_TECHNIQUE_DESC techDesc;
	ID3DX11EffectTechnique *tech = m_fx->GetTechniqueByName(techNameOss.str().c_str());
	tech->GetDesc(&techDesc);
	for(UINT i=0; i<techDesc.Passes; ++i)
	{
		tech->GetPassByIndex(i)->Apply(0,m_deviceContext);
		m_deviceContext->DrawIndexed(m_box.indices.size(),0,0);
	}

	m_swapChain->Present(0,0);

	return true;
}

bool TextureDemo::BuildTexture()
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,L"Texture/Wood.dds",0,0,&m_texView,0);
	if(FAILED(hr))
	{
		MessageBox(NULL,L"Create Texture failed!",L"Error",MB_OK);
		return false;
	}

	return true;
}

void TextureDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastPos.x = x;
	m_lastPos.y = y;

	SetCapture(m_hWnd);
}
	
void TextureDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
	
void TextureDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f*(m_lastPos.x - x));
		float dy = XMConvertToRadians(0.25f*(m_lastPos.y - y));

		m_theta += dx;
		m_phy += dy;

		m_phy = Clamp(0.01f,XM_PI-0.01f,m_phy);
	}
	else if((btnState & MK_RBUTTON) != 0)
	{
		float dRadius = 0.01f * static_cast<float>(x - m_lastPos.x);
		m_radius -= dRadius;

		m_radius = Clamp(4.f,300.f,m_radius);
	}

	m_lastPos.x = x;
	m_lastPos.y = y;
}

//程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	TextureDemo td(hInstance);
	if(!td.Init())
		return -1;

	return td.Run();
}
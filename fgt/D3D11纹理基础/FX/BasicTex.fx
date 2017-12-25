#include "Light.fx"

//针对单个物体的Effect全局变量
cbuffer PerObject
{
	float4x4	g_worldViewProj;		//世界、视角、投影变换
	float4x4	g_worldInvTranspose;	//世界变换的逆、转置矩阵
	float4x4	g_world;				//世界变换
};

//针对每一帧的Effect全局变量
cbuffer PerFrame
{
	SpotLight	g_spotLight;
	Material	g_material;
	float3		g_eyePos;
};

//纹理
Texture2D g_tex;

//设置过滤
SamplerState samTex
{
	Filter = MIN_MAG_MIP_LINEAR;
};

//输入顶点信息：位置坐标、法线、纹理坐标
struct VertexIn
{
	float3	pos:	POSITION;
	float3	normal:	NORMAL;
	float2	tex:	TEXCOORD;
};

//输出顶点信息
struct VertexOut
{
	float4	posH:	SV_POSITION;	//投影后坐标
	float3	pos:	POSITION;		//世界变换后坐标
	float3	normal:	NORMAL;			//世界变换后法线
	float2	tex:	TEXCOORD;		//纹理坐标
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.posH = mul(float4(vin.pos,1.f),g_worldViewProj);
	vout.pos = mul(float4(vin.pos,1.f),g_world).xyz;
	vout.normal = mul(vin.normal,(float3x3)g_worldInvTranspose);
	vout.tex = vin.tex;

	return vout;
}

float4 PS(VertexOut pin, uniform bool useLight): SV_TARGET
{
	float4 ambient = {1.f,1.f,1.f,1.f};
	float4 diffuse = {0.f,0.f,0.f,0.f};
	float4 spec = {0.f,0.f,0.f,0.f};

	//如果使用光照
	if(useLight)
	{
		float3 normal = normalize(pin.normal);
		float3 toEye = normalize(g_eyePos - pin.pos);

		ComputeSpotLight(g_material,g_spotLight,normal,pin.pos,toEye,ambient,diffuse,spec);
	}

	float4 texColor = g_tex.Sample(samTex,pin.tex);
	//纹理*(环境光+漫反射光）+全反射光，决定最终颜色
	float4 finalColor = texColor * (ambient+diffuse) + spec;

	return finalColor;
}

//使用光照的technique
technique11 TexLight
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0,VS()) );
		SetPixelShader( CompileShader(ps_5_0,PS(true)) );
	}
}

//不使用光照的technique
technique11 Tex
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0,VS()) );
		SetPixelShader( CompileShader(ps_5_0,PS(false)) );
	}
}
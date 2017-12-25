
cbuffer PerFrame
{
	float4x4	g_worldViewProj;
};

struct VertexIn
{
	float3	pos 	: POSITION;
	float4	color 	: COLOR;
};

struct VertexOut
{
	float4	posH	: SV_POSITION;
	float4	color	: COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.posH = mul(float4(vin.pos,1.f),g_worldViewProj);
	vout.color = vin.color;
	
	return vout;
}

float4 PS(VertexOut pin):SV_TARGET
{
	return pin.color;
}

technique11 BasicColor
{
	Pass p0
	{
		SetVertexShader(CompileShader(vs_4_0,VS()));
		SetPixelShader(CompileShader(ps_4_0,PS()));
	}
}

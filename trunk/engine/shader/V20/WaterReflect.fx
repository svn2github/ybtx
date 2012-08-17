/// @file		Water.fx
/// @date		2008/09/12   15:24
/// @version	1.0
/// @brief		water effect

float4x4	matWVP : register(c12);
float3	eyePos : register(c17);
float2	waveOffset0;
float2	waveOffset1;


sampler envMap : register(s0);
sampler waveMap0 : register(s1);
sampler waveMap1 : register(s2);

const float UV_SCALE = 1.0f / 1024;

struct VS_IN
{
	float3 position : POSITION;
	float4 diffuse	: COLOR0;
	float2 texCoord	: TEXCOORD0;
};

struct VS_OUT
{
	float4 position	: POSITION;
	float4 diffuse	: COLOR0;
	float3 eyeDir	: TEXCOORD0;
	float2 tex0		: TEXCOORD1;
	float2 tex1		: TEXCOORD2;
};

VS_OUT WaterVS(VS_IN input)
{
	VS_OUT output;
	output.position = mul(float4(input.position, 1.0f), matWVP);
	output.diffuse = input.diffuse;
    output.eyeDir = eyePos - input.position;
	input.texCoord = input.position.xz * UV_SCALE;
	output.tex0 = input.texCoord + waveOffset0;
	output.tex1 = input.texCoord + waveOffset1;
	
	return output;
}

struct PS_IN
{
	float3 eyeDir	: TEXCOORD0;
	float4 diffuse	: COLOR0;
	float2 tex0		: TEXCOORD1;
	float2 tex1		: TEXCOORD2;
};

float4 WaterPS(PS_IN input) : COLOR0
{
	float3 normal0 = tex2D(waveMap0, input.tex0);
	float3 normal1 = tex2D(waveMap1, input.tex1);
	// expand from [0,1] to [-1,1]
	normal0 = normal0 * 2.0f - 1.0f;
	normal1 = normal1 * 2.0f - 1.0f;
	// average the two normal vectors
	float3 normal = (normal0 + normal1) * 0.5f;
	float3 cubeDir = reflect(input.eyeDir, normal.yzx);
	return texCUBE(envMap, cubeDir) * input.diffuse;
}

technique WaterTech
{
	pass
	{
        AlphaTestEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = InvSrcAlpha;
		DestBlend = SrcAlpha;
        ZWriteEnable = false;
		vertexShader = compile vs_2_0 WaterVS();
		pixelShader = compile ps_2_0 WaterPS();
	}
}

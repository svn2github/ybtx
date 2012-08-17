/// @file		Water_ani.fx
/// @date		2008/11/19   16:29
/// @version	1.0
/// @brief		water effect

float4x4 matWVP : register(c12);
sampler	texWater : register(s0);
//sampler WaterWave = sampler_state
//{
//	Texture = <texWater>;
//	MinFilter = LINEAR;
//	MagFilter = LINEAR;
//	MipFilter = LINEAR;
//	AddressU  = WRAP;
//	AddressV  = WRAP;
//};


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
	float2 texCoord	: TEXCOORD0;
};

VS_OUT WaterVS(VS_IN input)
{
	VS_OUT output;
	output.position = mul(float4(input.position, 1.0f), matWVP);
	output.diffuse = input.diffuse;
	output.texCoord = input.texCoord;
	
	return output;
}

struct PS_IN
{
	float4 diffuse	: COLOR0;
	float2 texCoord	: TEXCOORD0;
};

float4 WaterPS(PS_IN input) : COLOR0
{
	return tex2D(texWater, input.texCoord) * input.diffuse;
}

technique WaterTech
{
	pass
	{
        AlphaTestEnable = false;
		AlphaBlendEnable = true;
		DestBlend = SrcAlpha;
	    SrcBlend = InvSrcAlpha;
        ZWriteEnable = false;
		vertexShader = compile vs_2_0 WaterVS();
		pixelShader = compile ps_2_0 WaterPS();
	}
}

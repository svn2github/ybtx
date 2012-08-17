float4x4	matWVP : register(c12);
float3	eyePos : register(c17);
float4	waveOffset : register(c100);

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
	input.texCoord *= UV_SCALE;
	output.tex0 = input.texCoord + waveOffset.xy;
	output.tex1 = input.texCoord + waveOffset.zw;

	return output;
}

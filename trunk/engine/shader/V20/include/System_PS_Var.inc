struct PS_INPUT
{
	float4 pos		: POSITION;
	float4 Diffuse	: COLOR0;
	float4 Specular	: COLOR1;
	float2 text0 : TEXCOORD0;
	float2 text1 : TEXCOORD1;
	float2 text2 : TEXCOORD2;
	float2 text3 : TEXCOORD3;
	float2 text4 : TEXCOORD4;
	float2 text5 : TEXCOORD5;
	float2 text6 : TEXCOORD6;
	float4 text7 : TEXCOORD7;
};
float4 gGlobalShadowColor : register(c30);

sampler Sampler0 : register(s0);
sampler Sampler1 : register(s1);
sampler Sampler2 : register(s2);
sampler Sampler3 : register(s3);
sampler ShadowSampler : register(s7);

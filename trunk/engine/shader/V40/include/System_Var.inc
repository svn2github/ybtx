#define MAX_MATRICES  76
#define MAX_LIGHTS 4 //更改这个的话, 下面的寄存器号也要改
//特效文件
float4x4 g_mProject : register(c0);
float4x4 g_mView : register(c4);
float4x4 g_mWorld : register(c8) ;
float4x4 g_mViewProj : register(c12) ;

float2 g_invViewSize : register(c16);
float3 g_vEyePos : register(c17);

float4   gMatDiffuse : register(c20);//模型漫反射颜色值
float4   gMatAmbient : register(c21);//模型全局光颜色值
float4   gMatSpecular : register(c22);
float4   gMatEmissive : register(c23);//模型全局光颜色值

float gRefractScalar : register(c24);
float4 gUVIndex : register(c25);

float4x4 gLightViewProject			: register(c31);//影子用

float4 gGlobalAmbient				: register(c35); //全局光照
float4 gGlobalLightColor			: register(c36);//太阳光漫反射颜色
float3 gGlobalLightDirection		: register(c37);//太阳光方向

//光源
float gLightCount					: register(c40);
float4 gLightColor[MAX_LIGHTS]		: register(c41);//点光源的diffuse系数
float4 gLightPosition[MAX_LIGHTS]	: register(c45);
float4 gLightDirection[MAX_LIGHTS]	: register(c49);
float4 gLightRangeInfo[MAX_LIGHTS]	: register(c53);
float4 gLightSpotInfo[MAX_LIGHTS]	: register(c57);

float2		Weight : register(c100); //顶点动画用
float2x4    mWorldMatrixArray[MAX_MATRICES]: register(c100) ;//骨骼动画用

struct VS_OUTPUT
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

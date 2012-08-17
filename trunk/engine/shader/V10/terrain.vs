#include "System_Var.inc"

//输入的顶点形式
struct VS_INPUT
{
     float3 pos       : POSITION;
     float3 normal	  : NORMAL;
     float4 color0	  : COLOR0;
     float4 color1	  : COLOR1;
     float2 text0     : TEXCOORD0;
     float2 text1     : TEXCOORD1;     
     float2 text2     : TEXCOORD2;
     float2 text3     : TEXCOORD3;  
     float2 text4     : TEXCOORD4;
};

//声明名为VS的顶点着色器
VS_OUTPUT Terrain_VS(VS_INPUT i)
{
    VS_OUTPUT 	output = (VS_OUTPUT)0;

	SYS_VERTEX_TRAN
	output.pos       = mul( float4(position, 1.0f), g_mViewProj ); 
      
    output.Diffuse  = i.color0;
    output.Specular = i.color1;
    output.text0 	= i.text0;
    output.text1 	= i.text1;
    output.text2 	= i.text2;
    output.text3 	= i.text3;
    output.text4 	= i.text4;
	SYS_TEXTURE_TRAN
    return output;	 
}

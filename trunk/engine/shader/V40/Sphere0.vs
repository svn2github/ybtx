#include "System_Var.inc"
#include "Lighting.inc"

SYS_INPUT_VAR

VS_OUTPUT VS_Sphere_0( VS_INPUT i )
{
	VS_OUTPUT 	output = (VS_OUTPUT)0;
	SYS_VERTEX_TRAN
	SYS_TRANSFORM_TRAN
	output.pos       = mul( float4(position.xyz, 1.0f), g_mViewProj ); 
	float3	viewNorm = mul( normal , g_mViewProj );
	viewNorm = normalize( viewNorm );
	output.text0	 = viewNorm.xy * float2( 0.5,0.5 )  + float2( 0.5,0.5 );	
	output.text1     = i.text[gUVIndex[1]];
	output.text2     = i.text[gUVIndex[2]];
	output.text3     = i.text[gUVIndex[3]];	
	SYS_LIGHT_TRAN
	SYS_DIFFUSE_TRAN
	SYS_TEXTURE_TRAN
	return output;
};

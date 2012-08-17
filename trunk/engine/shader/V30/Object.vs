#include "System_Var.inc"
#include "Lighting.inc"

SYS_INPUT_VAR

VS_OUTPUT VS_Main( VS_INPUT i )
{
	VS_OUTPUT 	output = (VS_OUTPUT)0;
	SYS_VERTEX_TRAN
	SYS_TRANSFORM_TRAN
	output.pos       = mul( float4(position.xyz, 1.0f), g_mViewProj ); 
	SYS_LIGHT_TRAN
	output.text0     = i.text[gUVIndex[0]];
	output.text1     = i.text[gUVIndex[1]];
	output.text2     = i.text[gUVIndex[2]];
	output.text3     = i.text[gUVIndex[3]];	
	SYS_DIFFUSE_TRAN
	SYS_TEXTURE_TRAN
	return output;
};

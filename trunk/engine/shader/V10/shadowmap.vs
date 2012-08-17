#include "System_Var.inc"
SYS_INPUT_VAR

VS_OUTPUT ShadowDepthVS(VS_INPUT i)
{
	VS_OUTPUT 	output = (VS_OUTPUT)0;
	SYS_VERTEX_TRAN
	SYS_TRANSFORM_TRAN
    output.pos = mul(float4(position, 1.0f), g_mViewProj);
	output.text0 = i.text[gUVIndex[0]];
    output.text1 = output.pos.zw;
    return output;
}

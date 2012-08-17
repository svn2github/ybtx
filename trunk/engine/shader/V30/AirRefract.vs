#include "System_Var.inc"
SYS_INPUT_VAR

VS_OUTPUT Air_Refract(VS_INPUT i)
{
	VS_OUTPUT 	output = (VS_OUTPUT)0;
	SYS_VERTEX_TRAN
	SYS_TRANSFORM_TRAN
    output.pos = mul(float4(position.xyz, 1.0f), g_mViewProj);
    output.Diffuse  = gMatDiffuse;
    output.text0.x = 0.5 + 0.5 * (output.pos.x / output.pos.w);
    output.text0.y = 0.5 - 0.5 * (output.pos.y / output.pos.w);
    float4x4 matWorldView = mul(g_mWorld, g_mView);
    float2 displace = normalize(mul(normal, matWorldView)).xy;
    displace.x = -displace.x;
    displace = normalize(displace) - displace;
    output.text0 += displace * g_invViewSize * gRefractScalar;// * i.color0.a;
    return output;
}


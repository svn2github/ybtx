//------------------------------------------------------------------------------
//  DistanceFont.fx
//  (C) 2008 ThreeOGCMan
//------------------------------------------------------------------------------
sampler texSampler0 : register(s0);
uniform float2 distanceRange;

float4 NormalPS(float4 clr : COLOR, float2 texCoord : TEXCOORD0) : COLOR0
{
	clr.a *= tex2D(texSampler0, texCoord).r * distanceRange.y + distanceRange.x;
	return clr;
}

float4 BoldPS(float4 clr : COLOR, float2 texCoord : TEXCOORD0) : COLOR0
{
	clr.a *= tex2D(texSampler0, texCoord).r * (distanceRange.y + 1) + distanceRange.x;
	return clr;
}

float4 OutlinePS(float4 clr : COLOR, float2 texCoord : TEXCOORD0) : COLOR0
{
	float luminance = tex2D(texSampler0, texCoord).r;
	float distance1 = luminance * distanceRange.y + distanceRange.x;
	float distance2 = luminance * (distanceRange.y + 3) + distanceRange.x;
	return float4(lerp(0, clr.rgb, distance1), distance2*clr.a);
}

float4 ShadowPS(float4 clr : COLOR, float2 texCoord : TEXCOORD0) : COLOR0
{
	float luminance = tex2D(texSampler0, texCoord - 0.0005).r;	//向左上偏移一个多像素
	float distance = luminance * (distanceRange.y + 1) + distanceRange.x;
	return float4(float3(0,0,0), distance*clr.a);
}

float4 GdiNormal(float4 clr : COLOR, float2 texCoord : TEXCOORD0) : COLOR0
{
    float luminance = tex2D(texSampler0, texCoord).r;
    return float4(clr.rgb, clr.a * luminance);
}

float4 GdiShadow(float4 clr : COLOR, float2 texCoord : TEXCOORD0) : COLOR0
{
    float luminance = tex2D(texSampler0, texCoord).r;
    return float4(clr.rgb * luminance, saturate(clr.a * luminance) * 4);
}

/// 正常字体
technique FontNormal
{
	pass
	{
		vertexShader = null;
		pixelShader = compile ps_2_0 NormalPS();
	}
}
/// 加粗
technique FontBold
{
	pass
	{
		vertexShader = null;
		pixelShader = compile ps_2_0 BoldPS();
	}
}
/// 描边
technique FontOutline
{
	pass
	{
		vertexShader = null;
		pixelShader = compile ps_2_0 OutlinePS();
	}
}
/// 投影
technique FontShadow
{
	pass shadow
	{
		vertexShader = null;
		pixelShader = compile ps_2_0 ShadowPS();
	}
	pass normal
	{
		vertexShader = null;
		pixelShader = compile ps_2_0 NormalPS();
	}
}

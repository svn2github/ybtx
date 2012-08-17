float2 invViewSize : register(c100);

struct VS_OUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD;
};

//------------------------------------------------------------------
// SCREEN QUAD
// simplely fill the screen with a quad
//------------------------------------------------------------------
VS_OUT ScreenQuad(float3 position : POSITION, float2 texCoord : TEXCOORD)
{
    VS_OUT output;
    position.xy += float2(-invViewSize.x, invViewSize.y);
    output.position = float4(position, 1);
    output.texCoord = texCoord;

    return output;
}

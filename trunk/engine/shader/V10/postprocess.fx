//------------------------------------------------------------------
//  GLOBAL VARIABLES
//------------------------------------------------------------------
float fBrightPassThreshold;             // Values greater than this are accepted for the bright pass
float4 tcDownSampleOffsets[16];         // The sampling offsets used by 'DownSample' and 'BrightPass'

float HBloomWeights[9];                 // Description of the sampling distribution used by
float HBloomOffsets[9];                 // the HorizontalBlur() function
float VBloomWeights[9];                 // Description of the sampling distribution used by
float VBloomOffsets[9];                 // the VerticalBlur() function

float fExposure;                        // A user configurable bias to under/over expose the image
float fScaler;                          // Blend scaler

float2 invViewSize;
texture texSource0 : register(t0);
texture texSource1 : register(t1);

sampler source0 : register(s0) : register(t0) = sampler_state {
    Texture = <texSource0>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
sampler source1 : register(s1) : register(t1) = sampler_state {
    Texture = <texSource1>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

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

//------------------------------------------------------------------
float4 Copy(float2 t : TEXCOORD0) : COLOR
{
    return tex2D(source0, t);
}
  
//------------------------------------------------------------------
// 4x4 DOWN-SAMPLING PIXEL SHADER
//
// Samples the input texture 16x according to the provided offsets
// and then writes the average to the output texture
//------------------------------------------------------------------
float4 DownSample( float2 t : TEXCOORD0 ) : COLOR
{
    float4 average = { 0.0f, 0.0f, 0.0f, 0.0f };

    for( int i = 0; i < 16; i++ )
    {
        average += tex2D( source0, t + float2(tcDownSampleOffsets[i].x, tcDownSampleOffsets[i].y) );
    }
    average *= ( 1.0f / 16.0f );

    return average;
}

//------------------------------------------------------------------
// BRIGHT PASS AND 2x2 DOWN-SAMPLING PIXEL SHADER
//
// performs the 2x2 down sample, and then accepts any pixels
// that are greater or equal to the configured threshold
//------------------------------------------------------------------
float4 Brightpass( float2 t : TEXCOORD0 ) : COLOR
{
    float4 average = { 0.0f, 0.0f, 0.0f, 0.0f };
    float2 offset = invViewSize.x * 0.125; // 0.5*invTexSize
    average += tex2D(source0, t + float2(offset.x, 0));
    average += tex2D(source0, t + float2(-offset.x, 0));
    average += tex2D(source0, t + float2(0, offset.y));
    average += tex2D(source0, t + float2(0, -offset.y));
    average *= 0.25f;
    float luminance = max( average.r, max( average.g, average.b ) );
    if( luminance < fBrightPassThreshold )
        average = float4( 0.0f, 0.0f, 0.0f, 1.0f );

    return average;
}
    
//------------------------------------------------------------------
// HORIZONTAL BLUR
//
// Takes 9 samples from the down-sampled texture (4 either side and
// one central) biased by the provided weights. Different weight
// distributions will give more subtle/pronounced blurring.
//------------------------------------------------------------------
float4 HorizontalBlur( float2 t : TEXCOORD0 ) : COLOR
{
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    for( int i = 0; i < 9; i++ )
    {
        color += tex2D( source0, t + float2( HBloomOffsets[i], 0.0f ) ) * HBloomWeights[i];
    }
        
    return float4( color.rgb, 1.0f );
}
   
//------------------------------------------------------------------
// VERTICAL BLUR
//
// Takes 9 samples from the down-sampled texture (4 above/below and
// one central) biased by the provided weights. Different weight
// distributions will give more subtle/pronounced blurring.
//------------------------------------------------------------------
float4 VerticalBlur( float2 t : TEXCOORD0 ) : COLOR
{
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    for( int i = 0; i < 9; i++ )
    {
        color += (tex2D( source0, t + float2( 0.0f, VBloomOffsets[i] ) ) * VBloomWeights[i] );
    }
        
    return float4( color.rgb, 1.0f );
}

//------------------------------------------------------------------
// COMBINE
// combine the original scene with bloomed scene
//------------------------------------------------------------------
float4 Combine( float2 t : TEXCOORD0 ) : COLOR0													
{																									
	float4 color = tex2D( source0, t );															
    float4 blur = tex2D(source1, t);
	float4 final = (1 - fScaler) * color + fScaler * blur;																	
	color = final * fExposure;																						

	return color;																						
};

//------------------------------------------------------------------------------
technique CopyPass
{
    pass
    {
        VertexShader = compile vs_2_0 ScreenQuad();
        PixelShader  = compile ps_2_0 Copy();
        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        ClipPlaneEnable  = 0;
    }
}

//------------------------------------------------------------------------------
technique DownSamplePass
{
    pass
    {
        VertexShader = compile vs_2_0 ScreenQuad();
        PixelShader  = compile ps_2_0 DownSample();
        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        ClipPlaneEnable  = 0;
    }
}
//------------------------------------------------------------------------------
technique BrightPass
{
    pass
    {
        VertexShader = compile vs_2_0 ScreenQuad();
        PixelShader  = compile ps_2_0 Brightpass();
        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        ClipPlaneEnable  = 0;
    }
}
//------------------------------------------------------------------------------
technique BlurVPass
{
    pass
    {
        VertexShader = compile vs_2_0 ScreenQuad();
        PixelShader  = compile ps_2_0 VerticalBlur();
        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        ClipPlaneEnable  = 0;
    }
}
//------------------------------------------------------------------------------
technique BlurHPass
{
    pass
    {
        VertexShader = compile vs_2_0 ScreenQuad();
        PixelShader  = compile ps_2_0 HorizontalBlur();
        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        ClipPlaneEnable  = 0;
    }
}
//------------------------------------------------------------------------------
technique CombinePass
{
    pass
    {
        VertexShader = compile vs_2_0 ScreenQuad();
        PixelShader  = compile ps_2_0 Combine();
        ZWriteEnable     = False;
        ZEnable          = False;
        AlphaBlendEnable = False;
        AlphaTestEnable  = False;
        CullMode         = None;
        ClipPlaneEnable  = 0;
    }
}
//------------------------------------------------------------------------------

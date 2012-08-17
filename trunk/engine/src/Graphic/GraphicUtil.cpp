#include "stdafx.h"
#include "GraphicUtil.h"
#include "TSqrAllocator.inl"
#define MAX_TRI_INDEXNUM	1024 //From D3D

DEFAULT1RS::DEFAULT1RS()
{
	m_AlpOP_S0 			= TOP_MODULATE;
}

PONITSAMPLERS::PONITSAMPLERS()
{
	m_Sample_S0 		= TEXF_POINT;
}

FONTRS::FONTRS()
{
	m_AlphaTestEnable	= FALSE;
	m_Alpharef			= 0x00;
	m_AlphaTestFun 		= CMP_GREATER;
	m_LightEnable 		= FALSE;
	m_AlphaBlendEnable	= TRUE;
	m_ZTestFun			= CMP_ALWAYS;
	m_Sample_S0 		= TEXF_POINT;
	m_TexOP_S0 			= TOP_SELECTARG2;
	m_TexArg2_S0		= TA_DIFFUSE;		
	m_AlpOP_S0 			= TOP_MODULATE;
	m_AlpArg1_S0		= TA_DIFFUSE;
	m_AlpArg2_S0		= TA_TEXTURE;
	m_UAddr_S0          = TADDRESS_CLAMP;
	m_VAddr_S0          = TADDRESS_CLAMP;
}

GUIRS::GUIRS()
{
	m_Alpharef			= 0x0000000;
	m_AlphaTestFun 		= CMP_GREATER;
	m_LightEnable 		= FALSE;
	m_ZTestFun			= CMP_ALWAYS;
	m_Sample_S0 		= TEXF_LINEAR;
	m_AlpOP_S0 			= TOP_MODULATE;
}

GUIMIRRORRS::GUIMIRRORRS()
{
	m_Alpharef			= 0x0000000;
	m_AlphaTestFun 		= CMP_GREATER;
	m_LightEnable 		= FALSE;
	m_ZTestFun			= CMP_ALWAYS;
	m_Sample_S0 		= TEXF_LINEAR;
	m_AlpOP_S0 			= TOP_MODULATE;
	m_UAddr_S0          = TADDRESS_CLAMP;
	m_VAddr_S0          = TADDRESS_CLAMP;
}

GUI_COLORADDRS::GUI_COLORADDRS()
{
	m_Alpharef			= 0x0000000;
	m_AlphaTestFun 		= CMP_GREATER;
	m_LightEnable 		= FALSE;
	m_ZTestFun			= CMP_ALWAYS;
	m_Sample_S0 		= TEXF_POINT;
	m_AlpOP_S0 			= TOP_MODULATE;
	m_DestBlend 		= BLEND_ONE;
}

MAP1LRS::MAP1LRS()
{
	//m_FillMode			= D3DFILL_WIREFRAME;
	m_TexOP_S0          = MODULATETYPE;
	m_TexOP_S1          = MODULATETYPE;
	m_AlphaBlendEnable	= FALSE;
	m_AlphaTestEnable	= FALSE;
	m_UVSrc_S0			= 0;
	m_UVSrc_S1			= 1;
	m_UVSrc_S2			= 2;
	m_UVSrc_S3			= 3;
	m_Uv_S0				= 0;
	m_Uv_S1				= 0;
	m_Uv_S2				= 0;
}

MAP2LRS::MAP2LRS()
{
	//m_FillMode			= D3DFILL_WIREFRAME;
	m_TexOP_S0          = MODULATETYPE;
	m_AlphaBlendEnable	= FALSE;
	m_AlphaTestEnable	= FALSE;
	m_LightEnable 		= TRUE;

	//m_TexOP_S0          = MODULATETYPE;
	m_TexOP_S1			= TOP_BLENDDIFFUSEALPHA;

	m_TexArg1_S1		= TA_TEXTURE;
	m_TexArg2_S1		= TA_CURRENT;
}

MAP4LRS::MAP4LRS()
{
	//m_FillMode			= D3DFILL_WIREFRAME;
	m_AlphaBlendEnable	= FALSE;
	m_AlphaTestEnable	= FALSE;
	m_LightEnable 		= TRUE;

	m_TexOP_S0          = MODULATETYPE;
	m_TexOP_S1			= TOP_BLENDDIFFUSEALPHA;
	m_TexArg1_S1		= TA_TEXTURE;
	m_TexArg2_S1		= TA_CURRENT;

	m_TexOP_S2			= TOP_BLENDDIFFUSEALPHA;
	m_TexArg1_S2		= TA_TEXTURE;
	m_TexArg2_S2		= TA_CURRENT;

	m_TexOP_S3			= TOP_BLENDDIFFUSEALPHA;
	m_TexArg1_S3		= TA_TEXTURE;
	m_TexArg2_S3		= TA_CURRENT;
}

SHADOWRS::SHADOWRS()
{
	m_ZTestFun			= CMP_ALWAYS;
	m_ZWrite				= FALSE;
	m_Texturefactor		= 0x5f000000;
	m_Alpharef			= 0x00000001;
	m_TexOP_S0			= TOP_SELECTARG1;
	m_TexArg1_S0		= TA_TFACTOR;
	m_AlpOP_S0			= TOP_MODULATE;
	m_AlpArg1_S0		= TA_TEXTURE;
	m_AlpArg2_S0		= TA_TFACTOR;
	m_Cull				= CULLT_NONE;
	m_AlphaTestEnable	= TRUE;
	m_MipMapBias_S0		= 100;
}

ALPHA2GRAYRS::ALPHA2GRAYRS()
{
	m_Alpharef			= 0x00000000;
	m_AlphaTestFun 		= CMP_GREATER;
	m_AlphaBlendEnable	= TRUE;
	m_LightEnable 		= FALSE;
	m_ZTestFun			= CMP_ALWAYS;
	m_Texturefactor		= 0xff000000;
	m_Material.Diffuse  = CColor4( 0xffffffff );
	m_TexOP_S0			= TOP_BLENDTEXTUREALPHA;
	m_TexArg1_S0		= TA_DIFFUSE;
	m_TexArg2_S0		= TA_TFACTOR;
	m_AlpOP_S0			= TOP_SELECTARG1;
	m_AlpArg1_S0		= TA_DIFFUSE;
}

NONETEXTALPHA::NONETEXTALPHA()
{
	m_AlpOP_S0 			= TOP_SELECTARG2;
}

NONETEXTALPHALINEAR::NONETEXTALPHALINEAR()
{
	m_Sample_S0 		= TEXF_LINEAR;
	m_UAddr_S0          = TADDRESS_CLAMP;
	m_VAddr_S0          = TADDRESS_CLAMP;
}

GUISHADOWRS::GUISHADOWRS()
{
	m_TexOP_S0			= TOP_SELECTARG2;
}

GUIALPHA::GUIALPHA()
{
	m_AlpOP_S0			= TOP_SELECTARG1;
	m_TexOP_S0			= TOP_SELECTARG1;
	m_AlpArg1_S0		= TA_TEXTURE;
	m_TexArg1_S0		= TA_TEXTURE;
	m_AlpOP_S1			= TOP_SELECTARG1;
	m_AlpArg1_S1		= TA_CURRENT;
	m_TexOP_S1			= TOP_BLENDCURRENTALPHA;
	m_TexArg1_S1		= TA_CURRENT;
	m_TexArg2_S1		= TA_TEXTURE;

#if 0
	m_AlpOP_S2			= TOP_SELECTARG1;
	m_AlpArg1_S2		= TA_DIFFUSE;

	m_TexOP_S2			= TOP_SELECTARG1;
	m_TexArg1_S2		= TA_CURRENT;
	m_ShaderType		= 0x00006019;
	m_AlphaBlendEnable	= TRUE;
#else
	m_AlphaBlendEnable	= FALSE;
#endif

	m_AlphaTestEnable	= TRUE;

	m_AlphaTestFun 		= CMP_GREATER;
	m_Alpharef			= 0x00000000;
	m_Uv_S0				= 0;
	m_Uv_S1				= 1;
}

GUIBIGMAP::GUIBIGMAP()
{
	m_AlpOP_S0			= TOP_SELECTARG1;
	m_AlpArg1_S0		= TA_TEXTURE;
	m_TexOP_S0			= TOP_SELECTARG1;
	m_TexArg1_S0		= TA_TEXTURE;

	m_AlpOP_S1			= TOP_DISABLE;
	m_TexOP_S1			= TOP_SELECTARG1;
	m_TexArg1_S1		= TA_TEXTURE;
	m_TexArg2_S1		= TA_CURRENT;

	m_AlpOP_S2			= TOP_DISABLE;
	m_TexOP_S2			= TOP_BLENDCURRENTALPHA;
	m_TexArg1_S2		= TA_TEXTURE;
	m_TexArg2_S2		= TA_CURRENT;

	m_AlphaBlendEnable	= FALSE;

	m_AlphaTestEnable	= FALSE;

	m_Uv_S0				= 0;
	m_Uv_S1				= 0;
	m_Uv_S2				= 0;
}

EMTRS::EMTRS()
{
	m_Alpharef			= 0x0000001;
	m_AlphaTestEnable	= TRUE;
	m_AlphaTestFun 		= CMP_GREATER;
	m_LightEnable 		= FALSE;
	m_ZTestFun			= CMP_ALWAYS;
	m_Sample_S0 		= TEXF_LINEAR;
	m_AlpOP_S0 			= TOP_MODULATE;

	m_TexOP_S0			= TOP_SELECTARG1;
	m_TexArg1_S0		= TA_TEXTURE;	
}

LINERS::LINERS()
{
	m_TexOP_S0			= TOP_SELECTARG1;
	m_TexArg1_S0		= TA_DIFFUSE;
	m_AlpOP_S0			= TOP_SELECTARG1;
	m_AlpArg1_S0		= TA_DIFFUSE;
	m_AlphaBlendEnable	= TRUE;
}

GUIALPHABLEND::GUIALPHABLEND()
{
	m_AlpOP_S0			= TOP_SELECTARG1;
	m_TexOP_S0			= TOP_SELECTARG1;
	m_AlpArg1_S0		= TA_TEXTURE;
	m_TexArg1_S0		= TA_TEXTURE;

	m_AlpOP_S1			= TOP_SELECTARG1;
	m_TexOP_S1			= TOP_SELECTARG1;

	m_AlpArg1_S1		= TA_CURRENT;

	m_TexArg1_S1		= TA_TEXTURE;
	//m_TexArg2_S1		= D3DTA_TEXTURE;

	m_AlphaBlendEnable	= TRUE;
	m_AlphaTestEnable	= TRUE;
	m_AlphaTestFun 		= CMP_GREATER;
	m_Alpharef			= 0x00000000;
	m_Uv_S0				= 0;
	m_Uv_S1				= 1;

#if 0
	m_ShaderType		= 0x00006018;
	m_AlphaBlendEnable	= TRUE;
#endif
}

TRIAGLE_INDEX::TRIAGLE_INDEX()
{
	m_IndexBuf.resize( MAX_TRI_INDEXNUM*6 );

	//Ìî³äË÷Òý»º³åÇø
	for ( WORD i = 0; i < MAX_TRI_INDEXNUM; i++ )
	{
		m_IndexBuf[ i*6 + 0 ] = i*4 + 0;
		m_IndexBuf[ i*6 + 1 ] = i*4 + 1;
		m_IndexBuf[ i*6 + 2 ] = i*4 + 2;
		m_IndexBuf[ i*6 + 3 ] = i*4 + 2;
		m_IndexBuf[ i*6 + 4 ] = i*4 + 1;
		m_IndexBuf[ i*6 + 5 ] = i*4 + 3;
	}
}

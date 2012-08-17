#include "stdafx.h"
#include "CRenderPipeline.h"
#include "CRenderOperation.h"
#include "CParamsBufferMgr.h"
#include "CShaderConstParams.h"
#include "CVertexDeclaration.h"
#include "CGraphicExp.h"
#include "CCamera.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

CRenderPipeline::CRenderPipeline()
		:// m_isShowFps(false)
		m_FogMode(FMT_LINEAR)
		, m_Ambient(0)
		, m_FogEnable(false)
		, m_FogColor(0)
		, m_fFogSatrt(0.0f)
		, m_fFogEnd(50000.0f)
		, m_pVectexDec(NULL)
		, m_fLimitFps(100.0f)
		, m_pCurGpuPro(NULL)
		, m_fZDelta(0.0f)
		, m_isNeedUpdateWorld(true)
		, m_pDefaultTex(NULL)
		, m_pMainBuffer(NULL)
{
	m_TextureMatrix.resize(8);
	m_bTexMatEnable.resize(8,false);
	m_TextBufMatrix.resize(8);
	m_BufStart.resize(8,0);
	m_BufEnable.resize(8,false);
}

CRenderPipeline::~CRenderPipeline()
{
	SafeRelease(m_pDefaultTex);
}

void CRenderPipeline::EnableFog( bool bEnable )
{
	m_FogEnable = bEnable;
	UpdateFog();
}

void CRenderPipeline::SetFog( uint32 Color, float Start, float End )
{
	m_FogMode = FMT_LINEAR;
	m_FogColor	= Color;
	m_fFogSatrt = Start;
	m_fFogEnd	= End;
}

void CRenderPipeline::SetFogColor( const uint32 Color )
{
	m_FogColor = Color;
}

void CRenderPipeline::SetLinerFogPara( const float fogstart, const float fogend )
{
	m_fFogSatrt = fogstart;
	m_fFogEnd	= fogend;
}

void CRenderPipeline::SetFogMode( FogModeType fogmode )
{
	m_FogMode = fogmode;
}

void CRenderPipeline::SetFogDensity( const float density )
{
	m_fFogDensity = density;
}

bool CRenderPipeline::IsFogEnabled(void)
{
	return m_FogEnable;
}

size_t	CRenderPipeline::GetFaceCount(void)
{
	return m_sFaceCount;
}

size_t	CRenderPipeline::GetVertexCount(void)
{
	return m_sVertexCount;
}

size_t	CRenderPipeline::GetPrimitiveCount(void)
{
	return m_sDrawPrimitiveCount;
}

void CRenderPipeline::SetCamera(CCamera* pCamera)
{
	AstMsg(pCamera, "摄像机必须有");
	m_CurProj = pCamera->getProjectionMatrix();
	m_CurView = pCamera->getViewMatrix() ;
	m_CurViewProj	= m_CurView * m_CurProj;
	SetVertexShaderF(ACT_CAMERA_POSITION,&pCamera->GetPosition(),1);
	SetVertexShaderF(ACT_PROJECTION_MATRIX,(float*)&m_CurProj,16);
	_UpdateView();
}

void CRenderPipeline::SetProject( const CMatrix& Project )
{
	m_CurProj = Project;
	m_CurViewProj	= m_CurView * m_CurProj;
	SetVertexShaderF(ACT_PROJECTION_MATRIX,(float*)&m_CurProj,16);
	_UpdateView();
}

void CRenderPipeline::SetView( const CMatrix& View )
{
	m_CurView = View;
	m_CurViewProj	= m_CurView * m_CurProj;
	SetVertexShaderF(ACT_PROJECTION_MATRIX,(float*)&m_CurProj,16);
	_UpdateView();
}

bool CRenderPipeline::Render(CRenderOperation* op,RenderParam* rp)
{
	Ast( NULL != rp);
	SetRenderStyle(rp->m_RS);
	SetWorld( rp->m_Mat );
	return RenderEx( op );
}

bool CRenderPipeline::BeginBatch(CRenderOperation* op,UINT I)
{
	m_sRenderVal = 0;
	if ( 0 == op->vertexData->vertexCount )
		return false;
	if ( op->useIndexes &&  0 == op->indexData->indexCount )
		return false;

	if (op->useIndexes)
		m_sRenderVal = op->indexData->indexCount;
	else
		m_sRenderVal = op->vertexData->vertexCount;

	switch (op->operationType)
	{
	case PT_TRIANGLELIST:
		m_sRenderVal /= 3;
		//m_sRenderVal *= I;
		m_sFaceCount += m_sRenderVal;
		break;
	case PT_TRIANGLESTRIP :
	case PT_TRIANGLEFAN:
		m_sRenderVal -= 2;
		//m_sRenderVal *= I;
		m_sFaceCount += m_sRenderVal - 2;
		break;
	case PT_LINELIST :
		m_sRenderVal /= 2;
		//m_sRenderVal *= I;
		break;
	case PT_LINESTRIP:
		m_sRenderVal -= 1;
		//m_sRenderVal *= I;
		break;
	case PT_POINTLIST:
		break;
	}

	if ( 0 == m_sRenderVal )
		return false;

	m_ActiveRenderOp = op;
	m_BatchNum = I;
	return _BeginBatch();
}

bool CRenderPipeline::SetBatchBuffer(UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag)
{
	if( m_ActiveRenderOp == NULL )
		return false;

	return _SetBatchBuffer(Source,pVBInfo,Flag);
}

bool CRenderPipeline::BatchRender(void)
{
	if ( 0 == m_sRenderVal || m_ActiveRenderOp == NULL )
		return false;
	m_sVertexCount += m_ActiveRenderOp->vertexData->vertexCount;
	return _Render(m_ActiveRenderOp);
}

bool CRenderPipeline::EndBatch(void)
{
	bool ret = _EndBatch();
	m_ActiveRenderOp = NULL;
	m_BatchNum = 0;
	m_pBinding = NULL;
	return ret;
}

bool CRenderPipeline::RenderEx(CRenderOperation* op)
{
	if ( 0 == op->vertexData->vertexCount )
		return true;
	if ( op->useIndexes &&  0 == op->indexData->indexCount )
		return true;

	if (op->useIndexes)
		m_sRenderVal = op->indexData->indexCount;
	else
		m_sRenderVal = op->vertexData->vertexCount;

	switch (op->operationType)
	{
	case PT_TRIANGLELIST:
		m_sRenderVal /= 3;
		m_sFaceCount += m_sRenderVal;
		break;
	case PT_TRIANGLESTRIP :
	case PT_TRIANGLEFAN:
		m_sRenderVal -= 2;
		m_sFaceCount += m_sRenderVal - 2;
		break;
	case PT_LINELIST :
		m_sRenderVal /= 2;
		break;
	case PT_LINESTRIP:
		m_sRenderVal -= 1;
		break;
	case PT_POINTLIST:
		break;
	}

	if ( 0 == m_sRenderVal )
		return true;

	m_sVertexCount += op->vertexData->vertexCount;
	m_pCurGpuPro = NULL;

	_setVertexDeclaration(op->vertexData->vertexDeclaration);
	_setVertexBufferBinding(op->vertexData->vertexBufferBinding);

	++m_sDrawPrimitiveCount;
	return _Render(op);
}

void CRenderPipeline ::SetShader(const SHADER_HANDLE& shaderID)
{
	SQR_TRY	
	{
		m_pCurGpuPro = m_pGpuMgr->bind( shaderID );
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		m_pCurGpuPro = NULL;
	}
	SQR_TRY_END;
}


void CRenderPipeline::SetWorld( const CMatrix& World )
{
	if(m_CurWorld!= World)
	{
		m_CurWorld = World;
		SetVertexShaderF(ACT_WORLD_MATRIX,(float*)&m_CurWorld,4);
		m_isNeedUpdateWorld = true;
	}
}

inline void CRenderPipeline::SetTexture( const STATIC_RS& RenderStyle )
{
	// 不用的纹理清空, 免得RTT和DST写入错误 [5/9/2009 xoyojank]
	_SetTexture( 0, RenderStyle.m_Texture_S0 );
	_SetTexture( 1, RenderStyle.m_Texture_S1 );
	_SetTexture( 2, RenderStyle.m_Texture_S2 );
	_SetTexture( 3, RenderStyle.m_Texture_S3 );
}

inline void CRenderPipeline::SetTextureEx( const STATIC_RS& RenderStyle )
{
	SetTexture( RenderStyle );
	_SetTextureStageState( 0, TSS_TEXCOORDINDEX, RenderStyle.m_ShaderType.IsValid() ? 0 : (RenderStyle.m_UVSrc_S0|RenderStyle.m_Uv_S0) );
	_SetTextureStageState( 1, TSS_TEXCOORDINDEX, RenderStyle.m_ShaderType.IsValid() ? 1 : (RenderStyle.m_UVSrc_S1|RenderStyle.m_Uv_S1) );
	_SetTextureStageState( 2, TSS_TEXCOORDINDEX, RenderStyle.m_ShaderType.IsValid() ? 2 : (RenderStyle.m_UVSrc_S2|RenderStyle.m_Uv_S2) );
	_SetTextureStageState( 3, TSS_TEXCOORDINDEX, RenderStyle.m_ShaderType.IsValid() ? 3 : RenderStyle.m_UVSrc_S3 );
	_SetTextureStageState( 0, TSS_COLOROP,  RenderStyle.m_TexOP_S0 );
	if ( RenderStyle.m_TexOP_S0 != TOP_DISABLE || RenderStyle.m_Texture_S0 != NULL )
	{
		_SetTextureStageState( 0, TSS_COLORARG1, RenderStyle.m_TexArg1_S0 );
		_SetTextureStageState( 0, TSS_COLORARG2, RenderStyle.m_TexArg2_S0 );
		_SetTextureStageState( 0, TSS_ALPHAOP,  RenderStyle.m_AlpOP_S0 );
		_SetTextureStageState( 0, TSS_ALPHAARG1, RenderStyle.m_AlpArg1_S0 );
		_SetTextureStageState( 0, TSS_ALPHAARG2, RenderStyle.m_AlpArg2_S0 );
		_SetSamplerState( 0, SAMP_MAGFILTER, RenderStyle.m_Sample_S0 );
		_SetSamplerState( 0, SAMP_MINFILTER, RenderStyle.m_Sample_S0 );
		_SetSamplerState( 0, SAMP_MIPFILTER, RenderStyle.m_MipMapSample_S0 );
		_SetSamplerState( 0, SAMP_MIPMAPLODBIAS,  RenderStyle.m_MipMapBias_S0 );
		_SetSamplerState( 0, SAMP_ADDRESSU, RenderStyle.m_UAddr_S0 );
		_SetSamplerState( 0, SAMP_ADDRESSV, RenderStyle.m_VAddr_S0 );
		_SetTextureStageState( 1, TSS_COLOROP, RenderStyle.m_TexOP_S1 );
		if ( RenderStyle.m_TexOP_S1 != TOP_DISABLE || RenderStyle.m_Texture_S1 != NULL )
		{
			_SetTextureStageState( 1, TSS_COLORARG1, RenderStyle.m_TexArg1_S1  );
			_SetTextureStageState( 1, TSS_COLORARG2, RenderStyle.m_TexArg2_S1 );
			_SetTextureStageState( 1, TSS_ALPHAOP,  RenderStyle.m_AlpOP_S1 );
			_SetTextureStageState( 1, TSS_ALPHAARG1, RenderStyle.m_AlpArg1_S1 );
			_SetTextureStageState( 1, TSS_ALPHAARG2, RenderStyle.m_AlpArg2_S1 );
			_SetSamplerState( 1, SAMP_MAGFILTER, RenderStyle.m_Sample_S1 );
			_SetSamplerState( 1, SAMP_MINFILTER, RenderStyle.m_Sample_S1 );
			_SetSamplerState( 1, SAMP_MIPFILTER, RenderStyle.m_MipMapSample_S1 );
			_SetSamplerState( 1, SAMP_MIPMAPLODBIAS, RenderStyle.m_MipMapBias_S1 );
			_SetSamplerState( 1, SAMP_ADDRESSU, RenderStyle.m_UAddr_S1 );
			_SetSamplerState( 1, SAMP_ADDRESSV, RenderStyle.m_VAddr_S1 );
			_SetTextureStageState( 2, TSS_COLOROP, RenderStyle.m_TexOP_S2 );

			if ( RenderStyle.m_TexOP_S2 != TOP_DISABLE || RenderStyle.m_Texture_S2 != NULL )
			{
				_SetTextureStageState( 2, TSS_COLORARG1, RenderStyle.m_TexArg1_S2 );
				_SetTextureStageState( 2, TSS_COLORARG2, RenderStyle.m_TexArg2_S2 );
				_SetTextureStageState( 2, TSS_ALPHAOP, RenderStyle.m_AlpOP_S2 );
				_SetTextureStageState( 2, TSS_ALPHAARG1, RenderStyle.m_AlpArg1_S2  );
				_SetTextureStageState( 2, TSS_ALPHAARG2, RenderStyle.m_AlpArg2_S2 );
				_SetSamplerState( 2, SAMP_MAGFILTER, RenderStyle.m_Sample_S2 );
				_SetSamplerState( 2, SAMP_MINFILTER, RenderStyle.m_Sample_S2 );
				_SetSamplerState( 2, SAMP_MIPFILTER, RenderStyle.m_MipMapSample_S2 );
				_SetSamplerState( 2, SAMP_MIPMAPLODBIAS, RenderStyle.m_MipMapBias_S2 );
				_SetSamplerState( 2, SAMP_ADDRESSU, RenderStyle.m_UAddr_S2 );
				_SetSamplerState( 2, SAMP_ADDRESSV, RenderStyle.m_VAddr_S2 );
				_SetTextureStageState( 3, TSS_COLOROP, RenderStyle.m_TexOP_S3 );

				if ( RenderStyle.m_TexOP_S3 != TOP_DISABLE || RenderStyle.m_Texture_S3 != NULL )
				{
					_SetTextureStageState( 3, TSS_COLORARG1, RenderStyle.m_TexArg1_S3 );
					_SetTextureStageState( 3, TSS_COLORARG2, RenderStyle.m_TexArg2_S3 );
					_SetTextureStageState( 3, TSS_ALPHAOP, RenderStyle.m_AlpOP_S3 );
					_SetTextureStageState( 3, TSS_ALPHAARG1, RenderStyle.m_AlpArg1_S3 );
					_SetTextureStageState( 3, TSS_ALPHAARG2, RenderStyle.m_AlpArg2_S3 );
					_SetSamplerState( 3, SAMP_MAGFILTER, RenderStyle.m_Sample_S3 );
					_SetSamplerState( 3, SAMP_MINFILTER, RenderStyle.m_Sample_S3 );
					_SetSamplerState( 3, SAMP_MIPFILTER, RenderStyle.m_MipMapSample_S3 );
					_SetSamplerState( 3, SAMP_MIPMAPLODBIAS, RenderStyle.m_MipMapBias_S3 );
					_SetSamplerState( 3, SAMP_ADDRESSU, RenderStyle.m_UAddr_S3 );
					_SetSamplerState( 3, SAMP_ADDRESSV, RenderStyle.m_VAddr_S3 );
				}//if( m_TexOP_S3 != TOP_DISABLE )
			}//if( m_TexOP_S2 != TOP_DISABLE )
		}//if( m_TexOP_S1 != TOP_DISABLE )
	}//if( m_TexOP_S0 != TOP_DISABLE )
}

void CRenderPipeline::ResetRenderState()
{
	_SetMaterial( m_GraphicRenderStyle.m_Material );
	_SetRenderStyle ( RS_ALPHAREF,			m_GraphicRenderStyle.m_Alpharef );
	_SetRenderStyle ( RS_TEXTUREFACTOR,		m_GraphicRenderStyle.m_Texturefactor );
	_SetRenderStyle ( RS_ALPHABLENDENABLE,	m_GraphicRenderStyle.m_AlphaBlendEnable );
	_SetRenderStyle ( RS_SRCBLEND,			m_GraphicRenderStyle.m_SrcBlend );
	_SetRenderStyle ( RS_DESTBLEND,			m_GraphicRenderStyle.m_DestBlend );
	_SetRenderStyle ( RS_ALPHATESTENABLE,	m_GraphicRenderStyle.m_AlphaTestEnable );
	_SetRenderStyle ( RS_ALPHAFUNC,			m_GraphicRenderStyle.m_AlphaTestFun );
	_SetRenderStyle ( RS_LIGHTING,			m_GraphicRenderStyle.m_LightEnable );
	_SetRenderStyle ( RS_SPECULARENABLE,	m_GraphicRenderStyle.m_SpecularEnable );
	_SetRenderStyle ( RS_ZENABLE,			m_GraphicRenderStyle.m_ZTestEnable );
	_SetRenderStyle ( RS_ZFUNC,				m_GraphicRenderStyle.m_ZTestFun );
	_SetRenderStyle ( RS_ZWRITEENABLE,		m_GraphicRenderStyle.m_ZWrite );
	_SetRenderStyle ( RS_CULLMODE,			m_GraphicRenderStyle.m_Cull );
	_SetRenderStyle ( RS_FILLMODE,			m_GraphicRenderStyle.m_FillMode );
	_SetTextureStageState( 0, TSS_COLOROP, m_GraphicRenderStyle.m_TexOP_S0 );
	_SetTexture( 0, m_GraphicRenderStyle.m_Texture_S0 );
	_SetTextureStageState( 0, TSS_COLORARG1, m_GraphicRenderStyle.m_TexArg1_S0 );
	_SetTextureStageState( 0, TSS_COLORARG2, m_GraphicRenderStyle.m_TexArg2_S0 );
	_SetTextureStageState( 0, TSS_ALPHAOP, m_GraphicRenderStyle.m_AlpOP_S0 );
	_SetTextureStageState( 0, TSS_ALPHAARG1, m_GraphicRenderStyle.m_AlpArg1_S0 );
	_SetTextureStageState( 0, TSS_ALPHAARG2, m_GraphicRenderStyle.m_AlpArg2_S0 );
	_SetSamplerState( 0, SAMP_MAGFILTER, m_GraphicRenderStyle.m_Sample_S0 );
	_SetSamplerState( 0, SAMP_MINFILTER, m_GraphicRenderStyle.m_Sample_S0 );
	_SetSamplerState( 0, SAMP_MIPFILTER, m_GraphicRenderStyle.m_MipMapSample_S0 );
	_SetSamplerState( 0, SAMP_MIPMAPLODBIAS, m_GraphicRenderStyle.m_MipMapBias_S0 );
	_SetTextureStageState( 0, TSS_TEXCOORDINDEX, m_GraphicRenderStyle.m_UVSrc_S0 );
	_SetSamplerState( 0, SAMP_ADDRESSU, m_GraphicRenderStyle.m_UAddr_S0 );
	_SetSamplerState( 0, SAMP_ADDRESSV, m_GraphicRenderStyle.m_VAddr_S0 );
	_SetTextureStageState( 1, TSS_COLOROP, m_GraphicRenderStyle.m_TexOP_S1 );
	_SetTexture( 1,  m_GraphicRenderStyle.m_Texture_S1 );
	_SetTextureStageState( 1, TSS_COLORARG1, m_GraphicRenderStyle.m_TexArg1_S1 );
	_SetTextureStageState( 1, TSS_COLORARG2, m_GraphicRenderStyle.m_TexArg2_S1 );
	_SetTextureStageState( 1, TSS_ALPHAOP, m_GraphicRenderStyle.m_AlpOP_S1 );
	_SetTextureStageState( 1, TSS_ALPHAARG1, m_GraphicRenderStyle.m_AlpArg1_S1 );
	_SetTextureStageState( 1, TSS_ALPHAARG2, m_GraphicRenderStyle.m_AlpArg2_S1 );
	_SetSamplerState( 1, SAMP_MAGFILTER, m_GraphicRenderStyle.m_Sample_S1 );
	_SetSamplerState( 1, SAMP_MINFILTER, m_GraphicRenderStyle.m_Sample_S1 );
	_SetSamplerState( 1, SAMP_MIPFILTER, m_GraphicRenderStyle.m_MipMapSample_S1 );
	_SetSamplerState( 1, SAMP_MIPMAPLODBIAS, m_GraphicRenderStyle.m_MipMapBias_S1 );
	_SetTextureStageState( 1, TSS_TEXCOORDINDEX, m_GraphicRenderStyle.m_UVSrc_S1 );
	_SetSamplerState( 1, SAMP_ADDRESSU, m_GraphicRenderStyle.m_UAddr_S1 );
	_SetSamplerState( 1, SAMP_ADDRESSV, m_GraphicRenderStyle.m_VAddr_S1 );
	_SetTextureStageState( 2, TSS_COLOROP, m_GraphicRenderStyle.m_TexOP_S2 );
	_SetTexture( 2,  m_GraphicRenderStyle.m_Texture_S2 );
	_SetTextureStageState( 2, TSS_COLORARG1, m_GraphicRenderStyle.m_TexArg1_S2 );
	_SetTextureStageState( 2, TSS_COLORARG2, m_GraphicRenderStyle.m_TexArg2_S2 );
	_SetTextureStageState( 2, TSS_ALPHAOP, m_GraphicRenderStyle.m_AlpOP_S2 );
	_SetTextureStageState( 2, TSS_ALPHAARG1, m_GraphicRenderStyle.m_AlpArg1_S2 );
	_SetTextureStageState( 2, TSS_ALPHAARG2, m_GraphicRenderStyle.m_AlpArg2_S2 );
	_SetSamplerState( 2, SAMP_MAGFILTER, m_GraphicRenderStyle.m_Sample_S2 );
	_SetSamplerState( 2, SAMP_MINFILTER, m_GraphicRenderStyle.m_Sample_S2 );
	_SetSamplerState( 2, SAMP_MIPFILTER, m_GraphicRenderStyle.m_MipMapSample_S2 );
	_SetSamplerState( 2, SAMP_MIPMAPLODBIAS, m_GraphicRenderStyle.m_MipMapBias_S2 );
	_SetTextureStageState( 2, TSS_TEXCOORDINDEX, m_GraphicRenderStyle.m_UVSrc_S2 );
	_SetSamplerState( 2, SAMP_ADDRESSU, m_GraphicRenderStyle.m_UAddr_S2 );
	_SetSamplerState( 2, SAMP_ADDRESSV, m_GraphicRenderStyle.m_VAddr_S2 );
	_SetTextureStageState( 3, TSS_COLOROP, m_GraphicRenderStyle.m_TexOP_S3 );
	_SetTexture( 3,  m_GraphicRenderStyle.m_Texture_S3 );
	_SetTextureStageState( 3, TSS_COLORARG1, m_GraphicRenderStyle.m_TexArg1_S3 );
	_SetTextureStageState( 3, TSS_COLORARG2, m_GraphicRenderStyle.m_TexArg2_S3 );
	_SetTextureStageState( 3, TSS_ALPHAOP, m_GraphicRenderStyle.m_AlpOP_S3 );
	_SetTextureStageState( 3, TSS_ALPHAARG1, m_GraphicRenderStyle.m_AlpArg1_S3 );
	_SetTextureStageState( 3, TSS_ALPHAARG2, m_GraphicRenderStyle.m_AlpArg2_S3 );
	_SetSamplerState( 3, SAMP_MAGFILTER, m_GraphicRenderStyle.m_Sample_S3 );
	_SetSamplerState( 3, SAMP_MINFILTER, m_GraphicRenderStyle.m_Sample_S3 );
	_SetSamplerState( 3, SAMP_MIPFILTER, m_GraphicRenderStyle.m_MipMapSample_S3 );
	_SetSamplerState( 3, SAMP_MIPMAPLODBIAS, m_GraphicRenderStyle.m_MipMapBias_S3 );
	_SetTextureStageState( 3, TSS_TEXCOORDINDEX, m_GraphicRenderStyle.m_UVSrc_S3 );
	_SetSamplerState( 3, SAMP_ADDRESSU, m_GraphicRenderStyle.m_UAddr_S3 );
	_SetSamplerState( 3, SAMP_ADDRESSV, m_GraphicRenderStyle.m_VAddr_S3 );

	_SetRenderStyle( RS_STENCILENABLE,		DWORD(m_GraphicStencilParam.m_bEnable) );
	_SetRenderStyle( RS_STENCILREF,			DWORD(m_GraphicStencilParam.m_nRefrence) );
	_SetRenderStyle( RS_STENCILFUNC,		m_GraphicStencilParam.m_nCompFun );
	_SetRenderStyle( RS_STENCILFAIL,		m_GraphicStencilParam.m_nFailedFlag );
	_SetRenderStyle( RS_STENCILZFAIL,		m_GraphicStencilParam.m_nZFailedFlag );
	_SetRenderStyle( RS_STENCILPASS,		m_GraphicStencilParam.m_nPassFlag );
	_SetRenderStyle( RS_STENCILMASK,		m_GraphicStencilParam.m_nReadMask );
	_SetRenderStyle( RS_STENCILWRITEMASK,	m_GraphicStencilParam.m_nWriteMask );

	_SetSamplerState( 0, SAMP_MAGFILTER, TEXF_LINEAR );
	_SetSamplerState( 1, SAMP_MAGFILTER, TEXF_LINEAR );
	_SetSamplerState( 2, SAMP_MAGFILTER, TEXF_LINEAR );
	_SetSamplerState( 3, SAMP_MAGFILTER, TEXF_LINEAR );
}

void CRenderPipeline::SetRenderStyle(const STATIC_RS& RenderStyle)	
{
	m_GraphicRenderStyle.m_Material = RenderStyle.m_Material;
	CRenderPipeline::GetInst()->SetShader(RenderStyle.m_ShaderType);
	_SetMaterial( m_GraphicRenderStyle.m_Material );
	_SetRenderStyle ( RS_ALPHAREF, RenderStyle.m_Alpharef );

	if (m_GraphicRenderStyle.m_Texturefactor!=RenderStyle.m_Texturefactor)
	{
		m_GraphicRenderStyle.m_Texturefactor = RenderStyle.m_Texturefactor;
		_SetRenderStyle ( RS_TEXTUREFACTOR, m_GraphicRenderStyle.m_Texturefactor );
		CColor4 factor = m_GraphicRenderStyle.m_Texturefactor;
		SetFragmentShaderF(ACT_TEXTURE_FACTOR, factor.r, factor.g, factor.b, factor.a);
	}

	float	Params[4];
	Params[0] = (float)RenderStyle.m_Alpharef;
	Params[1] = (float)RenderStyle.m_Material.Diffuse.a;
	Params[2] = (float)RenderStyle.m_Material.Specular.a;
	SetVertexShaderF(ACT_TEXTURE_FACTOR,Params,1);

	CColor4 tSpecular(0.0f,0.0f,0.0f,0.0f);
	if (RenderStyle.m_SpecularEnable)
	{
		tSpecular = m_GraphicRenderStyle.m_Material.Specular;
		tSpecular.a = m_GraphicRenderStyle.m_Material.Power;
	}

	SetVertexShaderF(ACT_MAT_DIFFUSE_COLOR	, (float*)&m_GraphicRenderStyle.m_Material.Diffuse, 1);
	SetVertexShaderF(ACT_MAT_AMBIENT_COLOR	, (float*)&m_GraphicRenderStyle.m_Material.Ambient, 1);
	SetVertexShaderF(ACT_MAT_SPECULAR_COLOR	, (float*)&tSpecular, 1);
	SetVertexShaderF(ACT_MAT_EMISSIVE_COLOR	, (float*)&m_GraphicRenderStyle.m_Material.Emissive, 1);
	
	SetVertexShaderF(ACT_REFRACT_SCALAR	, RenderStyle.m_fRefractIndex);
	SetVertexShaderF(ACT_UV_INDEX , float(RenderStyle.m_Uv_S0) , float(RenderStyle.m_Uv_S1) , float(RenderStyle.m_Uv_S2) );

	//_SetRenderStyle ( RS_SLOPESCALEDEPTHBIAS, *(DWORD*)&(RenderStyle.m_fSlopeScaleZBias));
	_SetRenderStyle ( RS_DEPTHBIAS, *(DWORD*)&(RenderStyle.m_fZBias));
	_SetRenderStyle ( RS_ALPHABLENDENABLE,  RenderStyle.m_AlphaBlendEnable );
	_SetRenderStyle ( RS_SRCBLEND, RenderStyle.m_SrcBlend );
	_SetRenderStyle ( RS_DESTBLEND, RenderStyle.m_DestBlend );
	_SetRenderStyle ( RS_ALPHATESTENABLE,  RenderStyle.m_AlphaTestEnable );
	_SetRenderStyle ( RS_ALPHAFUNC, RenderStyle.m_AlphaTestFun );
	_SetRenderStyle ( RS_LIGHTING, RenderStyle.m_LightEnable );
	_SetRenderStyle ( RS_SPECULARENABLE,  RenderStyle.m_SpecularEnable );
	_SetRenderStyle ( RS_ZENABLE,  RenderStyle.m_ZTestEnable );
	_SetRenderStyle ( RS_ZFUNC,  RenderStyle.m_ZTestFun );
	_SetRenderStyle ( RS_ZWRITEENABLE, RenderStyle.m_ZWrite );
	_SetRenderStyle ( RS_CULLMODE, RenderStyle.m_Cull );
	_SetRenderStyle ( RS_FILLMODE, RenderStyle.m_FillMode );
	bool isClipPlaneValid = RenderStyle.m_ClipPlane.IsValid();
	if (isClipPlaneValid)
	{
		CPlane plane = RenderStyle.m_ClipPlane;
		if (RenderStyle.m_ShaderType.GetVSShaderID())
		{
			// transform to clipping space
			CMatrix transform = m_CurViewProj;
			MatrixInverse(transform);
			transform.Transpose();
			plane.Transform(transform);
		}
		_SetClipPlane(0, plane);
	}
	_SetRenderStyle( RS_CLIPPLANEENABLE, DWORD(isClipPlaneValid ? CLIPPLANE0 : 0) );
	SetTextureEx(RenderStyle);
}

void CRenderPipeline::SetStencil( const StencilParam& Stencil )
{
	memcpy ( &m_GraphicStencilParam, &Stencil, sizeof( StencilParam ) );
	if ( Stencil.m_bEnable )
	{
		//if( Stencil.m_bEnable != m_Stencil.m_bEnable )
		_SetRenderStyle( RS_STENCILENABLE, DWORD(Stencil.m_bEnable) );
		//if( Stencil.m_nRefrence != m_Stencil.m_nRefrence )
		_SetRenderStyle( RS_STENCILREF,	DWORD(Stencil.m_nRefrence) );
		//if( Stencil.m_nCompFun != m_Stencil.m_nCompFun )
		_SetRenderStyle( RS_STENCILFUNC, Stencil.m_nCompFun );
		//if( Stencil.m_nFailedFlag != m_Stencil.m_nFailedFlag )
		_SetRenderStyle( RS_STENCILFAIL, Stencil.m_nFailedFlag );
		//if( Stencil.m_nZFailedFlag != m_Stencil.m_nZFailedFlag )
		_SetRenderStyle( RS_STENCILZFAIL, Stencil.m_nZFailedFlag );
		//if( Stencil.m_nPassFlag != m_Stencil.m_nPassFlag )
		_SetRenderStyle( RS_STENCILPASS, Stencil.m_nPassFlag );
		//if( Stencil.m_nReadMask != m_Stencil.m_nReadMask )
		_SetRenderStyle( RS_STENCILMASK, Stencil.m_nReadMask );
		//if( Stencil.m_nWriteMask != m_Stencil.m_nWriteMask )
		_SetRenderStyle( RS_STENCILWRITEMASK, Stencil.m_nWriteMask );
		//m_Stencil = Stencil;
	}
	else
	{
		//if( Stencil.m_bEnable == m_Stencil.m_bEnable )
		//	return;
		_SetRenderStyle( RS_STENCILENABLE,	DWORD(false) );
		//m_Stencil.m_bEnable = false;
	}
	return;
}

CMatrix* CRenderPipeline::GetTextureMat(uint32 uTextStage)
{
	if (m_BufEnable[uTextStage])
		return (CMatrix*)CParamsBufferMgr::GetInst()->getBuffer(m_BufStart[uTextStage]);
	else
		return NULL;
}

void CRenderPipeline::SetTextureMatrix( uint32 uTextStage, const CMatrix* pMatrix )
{
	if ( m_BufEnable[uTextStage] )
	{
		//有效的输入矩阵
		if ( pMatrix )
		{
			if ( *pMatrix != m_TextBufMatrix[uTextStage])
				(*(CMatrix*)CParamsBufferMgr::GetInst()->regBuffer(sizeof(CMatrix),m_BufStart[uTextStage]))=*pMatrix;
		}
		//无效的输入矩阵，禁用贴图坐标变换
		else
			m_BufEnable[uTextStage] = false;
	}
	//当前开关关闭
	else
	{
		//有效的输入矩阵
		if ( pMatrix )
		{
			if ( *pMatrix != m_TextBufMatrix[uTextStage] || m_BufEnable[uTextStage] == false)
				(*(CMatrix*)CParamsBufferMgr::GetInst()->regBuffer(sizeof(CMatrix),m_BufStart[uTextStage]))=*pMatrix;
			m_BufEnable[uTextStage] = true;
		}
		else
			return;
	}
}

void CRenderPipeline::_SetDefaultTexture(ITexture* pDefTex)
{
	if( m_pDefaultTex == pDefTex )
		return;

	SafeRelease(m_pDefaultTex);
	m_pDefaultTex = pDefTex;
	m_pDefaultTex->AddRef();
}
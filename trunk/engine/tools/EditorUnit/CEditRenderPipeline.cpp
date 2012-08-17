#include "stdafx.h"
#include "CEditRenderPipeline.h"
#include "CRenderOperation.h"
#include "CParamsBufferMgr.h"
#include "CShaderConstParams.h"
#include "CVertexDeclaration.h"
#include "CGraphicExp.h"
#include "CCamera.h"
#include "CEditorCore.h"

#include "CGraphic.h"

namespace sqr
{
	CEditRenderPipeline::CEditRenderPipeline( CRenderPipeline* pPipeline )
		: m_pipeline( static_cast<CRefRenderPipeline*>(pPipeline) )
		, bReFreshTexFactor(false)
	{
		assert(m_pipeline != 0);
		//基本参数统计
		IsBasicParamCount=true;
		//像素填充率统计
		IsPixelFillCount=false;
		//纹理统计
		IsTexCount=true;
		//纹理替换功能
		IsInsteadTex=false;
		InsteadTex=NULL;
	}

	CEditRenderPipeline::~CEditRenderPipeline()
	{
		m_pipeline = NULL;
	}

	CEditRenderPipeline* CEditRenderPipeline::GetInst()
	{
		return static_cast<CEditRenderPipeline*>(CRenderPipeline::GetInst());
	}

	void CEditRenderPipeline::BeginShader( const SHADER_HANDLE& shader )
	{
		m_ShaderHandle = shader;
		m_pipeline->SetShader(shader);
	}

	void CEditRenderPipeline::EndShader()
	{
		m_ShaderHandle.Clear();
	}

	void CEditRenderPipeline::SetVertexShaderF( uint id , float Param1 /*= 0.0f*/, float Param2 /*= 0.0f*/, 
			float Param3 /*= 0.0f*/, float Param4 /*= 0.0f */ )
	{
		m_pipeline->SetVertexShaderF( id, Param1, Param2, Param3, Param4 );
	}


	void CEditRenderPipeline::GetVertexShaderF( uint id, void* pdata, size_t float4Count )
	{
		m_pipeline->GetVertexShaderF( id, pdata, float4Count );
	}

	void CEditRenderPipeline::SetVertexShaderF( uint id, const void* pdata, size_t float4Count )
	{
		m_pipeline->SetVertexShaderF( id, pdata, float4Count );
	}

	void CEditRenderPipeline::SetFragmentShaderF( uint id, float Param1 /*= 0.0f*/, float Param2 /*= 0.0f*/, 
			float Param3 /*= 0.0f*/, float Param4 /*= 0.0f */ )
	{
		m_pipeline->SetFragmentShaderF( id, Param1, Param2, Param3, Param4 );
	}

	void CEditRenderPipeline::SetFragmentShaderF( uint id, const void* pdata, size_t float4Count )
	{
		m_pipeline->SetFragmentShaderF( id, pdata, float4Count );
	}

	void CEditRenderPipeline::GetFragmentShaderF( uint id, void* pdata, size_t float4Count )
	{
		m_pipeline->SetFragmentShaderF( id, pdata, float4Count );
	}

	void CEditRenderPipeline::_setVertexDeclaration( CVertexDeclaration* decl )
	{
		m_pipeline->_setVertexDeclaration(decl);
	}

	void CEditRenderPipeline::_setVertexBufferBinding( CVertexBufferBinding* binding )
	{
		m_pipeline->_setVertexBufferBinding(binding);
	}

	void CEditRenderPipeline::SetLight( index_t lightIndex, const LightStruct* light )
	{
		m_pipeline->SetLight( lightIndex, light );
	}

	void CEditRenderPipeline::SetTextureMatrixImmediate( uint32 uTextStage, const CMatrix* pMatrix )
	{
		m_pipeline->SetTextureMatrixImmediate( uTextStage, pMatrix );
	}

	void CEditRenderPipeline::InitPipeline( void )
	{
		m_pipeline->InitPipeline();
	}

	void CEditRenderPipeline::ReleasePipeline( void )
	{
		m_pipeline->ReleasePipeline();
	}

	bool CEditRenderPipeline::Begin( void )
	{	
		m_pTexSet.clear();
		TexNum=0;
		return m_pipeline->Begin();		
	}

	void CEditRenderPipeline::End( void )
	{
		m_pipeline->End();
	}

	void CEditRenderPipeline::ClearDepth( float depth /*= 1.0f */ )
	{
		m_pipeline->ClearDepth(depth);
	}

	void CEditRenderPipeline::ClearRender( const CColor& color )
	{
		m_pipeline->ClearRender(color);
	}

	void CEditRenderPipeline::ClearTarget( CRenderTarget* pTarget )
	{
		m_pipeline->ClearTarget(pTarget);
	}

	void CEditRenderPipeline::SetViewport( CIRect& Rect )
	{
		m_pipeline->SetViewport(Rect);
	}

	void CEditRenderPipeline::_SetRenderStyle( const RENDERSTATETYPE& rsType, DWORD fValue )
	{
		m_pipeline->_SetRenderStyle( rsType, fValue );
	}

	void CEditRenderPipeline::_SetSamplerState( uint i, const SAMPLERSTATETYPE& ssType, DWORD fValue )
	{
		m_pipeline->_SetSamplerState( i, ssType, fValue );
	}

	void CEditRenderPipeline::_SetTextureStageState( uint i, const TEXTURESTAGESTATETYPE& tsType, DWORD fValue )
	{
		m_pipeline->_SetTextureStageState( i, tsType, fValue );
	}

	void CEditRenderPipeline::_SetTexture( uint i, ITexture* pText )
	{
		//纹理统计
		if(IsTexCount)
		{	
			set<ITexture*>::iterator it=m_pTexSet.find(pText);
			if(it==m_pTexSet.end())
			{
				m_pTexSet.insert(it,pText);
				TexNum++;
			}
		}
		//纹理替换
		if(IsInsteadTex && i==0 )
		{
			m_pipeline->_SetTexture( i, InsteadTex );
		}
		else
			m_pipeline->_SetTexture( i, pText );
		return ;
	}

	void CEditRenderPipeline::_SetMaterial( const sMaterial& material )
	{
		m_pipeline->_SetMaterial( material );
	}

	void CEditRenderPipeline::_SetClipPlane( uint i,const CPlane& Plane )
	{
		m_pipeline->_SetClipPlane( i, Plane );
	}

	void CEditRenderPipeline::SetCamera( CCamera* pCamera )
	{
		m_pipeline->SetCamera(pCamera);

		m_CurProj = pCamera->getProjectionMatrix();
		m_CurView = pCamera->getViewMatrix() ;
		m_CurViewProj	= m_CurView * m_CurProj;
	}

	void CEditRenderPipeline::SetProject( const CMatrix& Project )
	{
		m_pipeline->SetProject(Project);

		m_CurProj = Project;
		m_CurViewProj	= m_CurView * m_CurProj;
	}

	void CEditRenderPipeline::SetView( const CMatrix& View )
	{
		m_pipeline->SetView(View);

		m_CurView = View;
		m_CurViewProj	= m_CurView * m_CurProj;
	}

	void CEditRenderPipeline::SetWorld( const CMatrix& World )
	{
		m_pipeline->SetWorld(World);

		if(m_CurWorld!= World)
			m_CurWorld = World;
	}

	bool CEditRenderPipeline::Render( CRenderOperation* op,RenderParam* rp )
	{
		Ast( NULL != rp);
		SetRenderStyle(rp->m_RS);
		SetWorld( rp->m_Mat );

		if(IsPixelFillCount)
		{
			StencilParam tmpStencilParam;
			tmpStencilParam.m_bEnable=TRUE;
			tmpStencilParam.m_nCompFun=CMP_ALWAYS;
			tmpStencilParam.m_nRefrence=0;
			tmpStencilParam.m_nFailedFlag=STENCILOP_KEEP;
			tmpStencilParam.m_nZFailedFlag=STENCILOP_KEEP;
			tmpStencilParam.m_nPassFlag=STENCILOP_INCRSAT;
			tmpStencilParam.m_nReadMask=0xFFFFFFFF;
			tmpStencilParam.m_nWriteMask=0xFFFFFFFF;
			//开启模板测试
			_SetRenderStyle( RS_STENCILENABLE,		DWORD(tmpStencilParam.m_bEnable) );
			_SetRenderStyle( RS_STENCILREF,			DWORD(tmpStencilParam.m_nRefrence) );
			_SetRenderStyle( RS_STENCILFUNC,		tmpStencilParam.m_nCompFun );
			_SetRenderStyle( RS_STENCILFAIL,		tmpStencilParam.m_nFailedFlag );
			_SetRenderStyle( RS_STENCILZFAIL,		tmpStencilParam.m_nZFailedFlag );
			_SetRenderStyle( RS_STENCILPASS,		tmpStencilParam.m_nPassFlag );
			_SetRenderStyle( RS_STENCILMASK,		tmpStencilParam.m_nReadMask );
			_SetRenderStyle( RS_STENCILWRITEMASK,	tmpStencilParam.m_nWriteMask );
			//绑定像素填充率计算的shader
		}
		else
		{
			//将模版缓存恢复到原来状态!即RS中 但是引擎没有封装,而且也不适合每一桢都调用 
			//所有将模版恢复到原来状态的设置代码写在外面 
		}

		return RenderEx( op );
	}

	bool CEditRenderPipeline::RenderEx( CRenderOperation* op )
	{
		if(IsBasicParamCount)
			return m_pipeline->RenderEx(op);
		else
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
				break;
			case PT_TRIANGLESTRIP :
			case PT_TRIANGLEFAN:
				m_sRenderVal -= 2;
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

			m_pipeline->m_sRenderVal = m_sRenderVal;
			m_pipeline->m_pCurGpuPro = NULL;

			m_pipeline->_setVertexDeclaration(op->vertexData->vertexDeclaration);
			m_pipeline->_setVertexBufferBinding(op->vertexData->vertexBufferBinding);
			
			return m_pipeline->_Render(op);
		}
	
	}

	bool CEditRenderPipeline::BeginBatch( CRenderOperation* op,UINT I )
	{
		return m_pipeline->BeginBatch(op, I);
	}

	bool CEditRenderPipeline::SetBatchBuffer( UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag )
	{
		return m_pipeline->SetBatchBuffer( Source, pVBInfo, Flag );
	}

	bool CEditRenderPipeline::BatchRender( void )
	{
		return m_pipeline->BatchRender();
	}

	bool CEditRenderPipeline::EndBatch( void )
	{
		return m_pipeline->EndBatch();
	}

	void CEditRenderPipeline::SetTexture( const STATIC_RS& RenderStyle )
	{
		//m_pipeline->SetTexture( RenderStyle );
		//return ;
		_SetTexture( 0, RenderStyle.m_Texture_S0 );
		_SetTexture( 1, RenderStyle.m_Texture_S1 );
		_SetTexture( 2, RenderStyle.m_Texture_S2 );
		_SetTexture( 3, RenderStyle.m_Texture_S3 );
	}

	void CEditRenderPipeline::SetTextureEx( const STATIC_RS& RenderStyle )
	{
		//m_pipeline->SetTextureEx(RenderStyle);
		//return ;
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

	void CEditRenderPipeline::SetRenderStyle( const STATIC_RS& RenderStyle )
	{
		//m_pipeline->SetRenderStyle(RenderStyle);
		//return ;

		m_GraphicRenderStyle.m_Material = RenderStyle.m_Material;
		CRenderPipeline::GetInst()->SetShader(RenderStyle.m_ShaderType);
		_SetMaterial( m_GraphicRenderStyle.m_Material );
		_SetRenderStyle ( RS_ALPHAREF, RenderStyle.m_Alpharef );

		if (m_GraphicRenderStyle.m_Texturefactor!=RenderStyle.m_Texturefactor || bReFreshTexFactor)
		{
			m_GraphicRenderStyle.m_Texturefactor = RenderStyle.m_Texturefactor;
			_SetRenderStyle ( RS_TEXTUREFACTOR, m_GraphicRenderStyle.m_Texturefactor );
			CColor4 factor = m_GraphicRenderStyle.m_Texturefactor;
			SetFragmentShaderF(ACT_TEXTURE_FACTOR, factor.r, factor.g, factor.b, factor.a);
			bReFreshTexFactor = false;
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

	void CEditRenderPipeline::ResetRenderState()
	{
		m_pipeline->ResetRenderState();
	}

	void CEditRenderPipeline::SetShader( const SHADER_HANDLE& shaderID )
	{
		if ( m_ShaderHandle.IsNull() )
			m_pipeline->SetShader(shaderID);
	}

	void CEditRenderPipeline::SetStencil( const StencilParam& Stencil )
	{
		m_pipeline->SetStencil(Stencil);
	}

	void CEditRenderPipeline::SetTextureMatrix( uint32 uTextStage, const CMatrix* pMatrix )
	{
		m_pipeline->SetTextureMatrix(uTextStage, pMatrix);
	}

	CMatrix* CEditRenderPipeline::GetTextureMat( uint32 uTextStage )
	{
		return m_pipeline->GetTextureMat(uTextStage);
	}

	bool CEditRenderPipeline::_BeginBatch( void )
	{
		return false;
	}

	bool CEditRenderPipeline::_SetBatchBuffer( UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag )
	{
		Source = 0; pVBInfo = 0; Flag = 0;
		return false;
	}

	bool CEditRenderPipeline::_EndBatch( void )
	{
		return false;
	}

	void CEditRenderPipeline::_UpdateView( void )
	{
	}

	bool CEditRenderPipeline::_Render( CRenderOperation* op )
	{
		op = 0;
		return false;
	}

	bool CEditRenderPipeline::boxProbe( const CVector3f& Min, const CVector3f& Max, 
			const CVector3f& RayPosition, const CVector3f& RayDirection )
	{
		return m_pipeline->boxProbe( Min, Max, RayPosition, RayDirection );
	}

	CMatrix& CEditRenderPipeline::MatrixInverse( CMatrix& matrix )
	{
		return m_pipeline->MatrixInverse(matrix);
	}

	size_t CEditRenderPipeline::GetFaceCount( void )
	{
		return m_pipeline->GetFaceCount();
	}

	size_t CEditRenderPipeline::GetVertexCount( void )
	{
		return m_pipeline->GetVertexCount();
	}

	size_t CEditRenderPipeline::GetPrimitiveCount( void )
	{
		return m_pipeline->GetPrimitiveCount();
	}

	void CEditRenderPipeline::EnableFog( bool bEnable )
	{
		m_pipeline->EnableFog(bEnable);
	}

	void CEditRenderPipeline::SetFog( uint32 Color, float Start, float End )
	{
		m_pipeline->SetFog(Color, Start, End);

		m_FogMode = FMT_LINEAR;
		m_FogColor	= Color;
		m_fFogSatrt = Start;
		m_fFogEnd	= End;
	}

	void CEditRenderPipeline::SetFogColor( const uint32 Color )
	{
		m_pipeline->SetFogColor(Color);
		m_FogColor = Color;
	}

	void CEditRenderPipeline::SetLinerFogPara( const float fogstart, const float fogend )
	{
		m_pipeline->SetLinerFogPara(fogstart, fogend);

		m_fFogSatrt = fogstart;
		m_fFogEnd	= fogend;
	}

	void CEditRenderPipeline::SetFogMode( FogModeType fogmode )
	{
		m_pipeline->SetFogMode(fogmode);
		m_FogMode = fogmode;
	}

	void CEditRenderPipeline::SetFogDensity( const float density )
	{
		m_pipeline->SetFogDensity(density);
		m_fFogDensity = density;
	}

	bool CEditRenderPipeline::IsFogEnabled( void )
	{
		return m_pipeline->IsFogEnabled();
	}

	void CEditRenderPipeline::UpdateFog()
	{
		m_pipeline->UpdateFog();
	}

	void CEditRenderPipeline::SetZDelta( float Z )
	{
		m_pipeline->SetZDelta(Z);
	}

	void CEditRenderPipeline::SetAmbient( uint32 Color )
	{
		m_pipeline->SetAmbient(Color);
	}

	void CEditRenderPipeline::SetAmbientSrcAsMaterial( bool bMaterial )
	{
		m_pipeline->SetAmbientSrcAsMaterial(bMaterial);
	}

	void CEditRenderPipeline::SetLimitFps( float fLFps )
	{
		m_pipeline->SetLimitFps(fLFps);
		m_fLimitFps = fLFps;
	}

	void CEditRenderPipeline::_SetDefaultTexture( ITexture* pDefTex )
	{
		m_pipeline->_SetDefaultTexture(pDefTex);
	}
	
	//new add
	//基本参数统计开关
	void CEditRenderPipeline::EnableBasicParamCount()
	{
		IsBasicParamCount=true;
	}
	void CEditRenderPipeline::DisableBasicParamCount()
	{
		IsBasicParamCount=false;
	}
	//像素填充率控制
	void CEditRenderPipeline::EnablePixelFillCount()
	{
		IsPixelFillCount=true;
	}

	void CEditRenderPipeline::DisablePixelFillCount()
	{
		IsPixelFillCount=false;
	}

	void CEditRenderPipeline::EnableTexInstead(ITexture* tex)
	{
		IsInsteadTex=true;
		if(tex)
			InsteadTex=tex;
	}
	//纹理统计功能
	void CEditRenderPipeline::EnalbeTexCount()
	{
		IsTexCount=true;
	}
	void CEditRenderPipeline::DisableTexCount()
	{
		IsTexCount=false;
	}
	//纹理替换控制
	void CEditRenderPipeline::DisableTexInstead()
	{
		IsInsteadTex=false;
	}

	void CEditRenderPipeline::UseColorInsteadTex()
	{
		InsteadTex = CEditorCore::TP_TX_BLACKWHITE;
	}

	void CEditRenderPipeline::UseGridInsteadTex()
	{
		InsteadTex = CEditorCore::TP_TX_GRAY;
	}

	bool CEditRenderPipeline::IsEnableTexInstead()
	{
		return IsInsteadTex;
	}

	void CEditRenderPipeline::SetReFreshTexFactor(bool b)
	{
		bReFreshTexFactor = b;
	}

	size_t CEditRenderPipeline::GetTexCount(void)
	{
		return TexNum;
	}

	void CEditRenderPipeline::GetCurFrameSceneTextureList( vector<ITexture *>& texNameList )
	{
		string name("");
		size_t size = m_pTexSet.size();

		ITexture *pTex = NULL;
		set<ITexture*>::iterator beg = m_pTexSet.begin();
		set<ITexture*>::iterator end = m_pTexSet.end();
		for ( beg; beg != end; ++beg )
		{
			pTex = *beg;

			if( pTex == NULL )
				continue;
			texNameList.push_back(pTex);
		}
	}
}
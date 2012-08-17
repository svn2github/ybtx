#include "stdafx.h"
#include "CDX9RenderPipeline.h"
#include "CRenderOperation.h"
#include "CDX9VertexHardwareBuffer.h"
#include "CDX9IndexHardwareBuffer.h"
#include "CDX9VertexDeclaration.h"
#include "CParamsBufferMgr.h"
#include <MMSystem.h>
#include <time.h>
#include "float.h"
#include "BaseHelper.h"

#include "CCamera.h"
#include "CGraphicExp.h"
#include "CShaderConstParams.h"
#include "CRenderStyle.h"
#include "CDx9MainWindow.h"
#include "DXErr.h"
#include "CGraphic.h"

#include "StringHelper.h"
namespace sqr
{

	DX9RenderPipeline::DX9RenderPipeline(void)
		: m_pD3DDevice(NULL)
		, m_pQuery(NULL)
		, m_pQueryOcclusion(NULL)
	{
	}

	DX9RenderPipeline::~DX9RenderPipeline()
	{
	}

	void DX9RenderPipeline::SetVertexShaderF(uint id,float Param1,float Param2,float Param3,float Param4)
	{
		if ( (id) > 255 )
			GenErr(GraphicErr::GetErrTypeStr(ShaderRuntime_Err),"GPU寄存器越界");
		m_pD3DDevice->SetVertexShaderConstantF(id, &Param1, 1);
	}

	void DX9RenderPipeline::SetVertexShaderF(uint id,const void* pdata,size_t float4Count)
	{
		if ( (id+float4Count) > 255 )
		{
			GenErr(GraphicErr::GetErrTypeStr(ShaderRuntime_Err),"GPU寄存器越界");
		}

		m_pD3DDevice->SetVertexShaderConstantF(id, (float*)pdata, float4Count);
	}

	void DX9RenderPipeline::GetVertexShaderF(uint id,void* pdata,size_t float4Count)
	{
		if ( (id+float4Count) > 255 )
		{
			GenErr(GraphicErr::GetErrTypeStr(ShaderRuntime_Err),"GPU寄存器越界");
		}

		m_pD3DDevice->GetVertexShaderConstantF(id, (float*)pdata, float4Count);
	}

	void DX9RenderPipeline::SetFragmentShaderF(uint id,float Param1,float Param2,float Param3,float Param4)
	{
		if ( (id) > 255 )
			GenErr(GraphicErr::GetErrTypeStr(ShaderRuntime_Err),"GPU寄存器越界");
		m_pD3DDevice->SetPixelShaderConstantF(id, &Param1, 1);
	}


	void DX9RenderPipeline::SetFragmentShaderF(uint id,const void* pdata,size_t float4Count)
	{
		if ( (id+float4Count) > 32 )
		{
			GenErr(GraphicErr::GetErrTypeStr(ShaderRuntime_Err),"GPU寄存器越界");
		}

		m_pD3DDevice->SetPixelShaderConstantF(id, (float*)pdata, float4Count);
	}

	void DX9RenderPipeline::GetFragmentShaderF(uint id,void* pdata,size_t float4Count)
	{
		if ( (id+float4Count) > 32 )
		{
			GenErr(GraphicErr::GetErrTypeStr(ShaderRuntime_Err),"GPU寄存器越界");
		}

		m_pD3DDevice->GetPixelShaderConstantF(id, (float*)pdata, float4Count);
	}

	void DX9RenderPipeline::_UpdateView(void)
	{
		m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &get(m_CurProj) );
		m_pD3DDevice->SetTransform( D3DTS_VIEW, &get(m_CurView) );
	}

	void DX9RenderPipeline::_SetRenderStyle( const RENDERSTATETYPE& rsType, DWORD fValue )
	{
		m_pD3DDevice->SetRenderState ( get(rsType) , fValue );
	}

	void DX9RenderPipeline::_SetSamplerState( uint i,const SAMPLERSTATETYPE& ssType, DWORD fValue )
	{
		m_pD3DDevice->SetSamplerState ( i , get(ssType) , fValue );
	}

	void DX9RenderPipeline::_SetTextureStageState( uint i,const TEXTURESTAGESTATETYPE& tsType, DWORD fValue )
	{
		m_pD3DDevice->SetTextureStageState ( i , get(tsType) , fValue );
	}

	void DX9RenderPipeline::_SetTexture( uint i, ITexture* pText )
	{
		
		UINT Flag = 0;
		SQR_TRY
		{
			LPDIRTEX tpText = pText?(LPDIRTEX)pText->GetBuffer() : NULL;

			if ( NULL == tpText && NULL != m_pDefaultTex )
			{
				Flag = 1;
				tpText = (LPDIRTEX)m_pDefaultTex->GetBuffer();
			}

			Flag = 2;
			m_pD3DDevice->SetTexture( i, tpText );
		}
		SQR_CATCH(exp)
		{
			switch(Flag)
			{
			case 0:
				exp.AppendType("_ST:GetBuffer");
				break;
			case 1:
				exp.AppendType("_ST:GetDefault");
				break;
			case 2:				
			default:
				exp.AppendType("_ST:SetD3D");
			}
			
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}

	void DX9RenderPipeline::_SetMaterial( const sMaterial& material )
	{
		m_pD3DDevice->SetMaterial( &get(material) );
	}

	void DX9RenderPipeline::_SetClipPlane( uint i,const CPlane& Plane )
	{
		m_pD3DDevice->SetClipPlane( i, (float*)&Plane );
	}

	bool DX9RenderPipeline::_BeginBatch(void)
	{
		_setVertexDeclaration(m_ActiveRenderOp->vertexData->vertexDeclaration);
		m_pBinding = m_ActiveRenderOp->vertexData->vertexBufferBinding;	
		const CVertexBufferBinding::VertexBufferBindingMap& binds = m_pBinding->GetBindings();
		CVertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend = binds.end();
		
		size_t source = 0;

		for (i = binds.begin(); i != iend; ++i, ++source)
		{
			for ( ; source < i->first; ++source)
				m_pD3DDevice->SetStreamSource(static_cast<uint>(source), NULL, 0, 0);

			DX9VertexHardwareBuffer* d3d9buf =  (DX9VertexHardwareBuffer*)i->second.m_pBuffer;
			m_pD3DDevice->SetStreamSourceFreq(static_cast<uint>(source),
				(D3DSTREAMSOURCE_INDEXEDDATA | m_BatchNum));
			m_pD3DDevice->SetStreamSource(
				static_cast<uint>(source),
				d3d9buf->getD3D9VertexBuffer(),
				static_cast<uint>(i->second.m_sOffset),
				static_cast<uint>(i->second.m_sVertexSize));
		}
		m_pBinding->Update();
		return true;
	}

	bool DX9RenderPipeline::_SetBatchBuffer(UINT Source, BufferBindingInfo* pVBInfo, DWORD Flag)
	{
		DX9VertexHardwareBuffer* d3d9buf =  (DX9VertexHardwareBuffer*)pVBInfo->m_pBuffer;
		m_pD3DDevice->SetStreamSourceFreq(static_cast<uint>(Source),
			(Flag | 1));
		m_pD3DDevice->SetStreamSource(
			static_cast<uint>(Source),
			d3d9buf->getD3D9VertexBuffer(),
			static_cast<uint>(pVBInfo->m_sOffset),
			static_cast<uint>(pVBInfo->m_sVertexSize));
		return true;
	}

	bool DX9RenderPipeline::_EndBatch(void)
	{	
		size_t source = 0;
		const CVertexBufferBinding::VertexBufferBindingMap& binds = m_pBinding->GetBindings();
		CVertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend = binds.end();
		for (i = binds.begin(); i != iend; ++i, ++source)
			m_pD3DDevice->SetStreamSourceFreq(static_cast<uint>(source),1);
		return true;
	}

	bool DX9RenderPipeline::_Render(CRenderOperation* op)
	{
		AstMsg(m_pD3DDevice, "Render必须在Bengin ,End之间使用");
		if(m_isNeedUpdateWorld)
		{
			m_pD3DDevice->SetTransform( D3DTS_WORLD, &get(m_CurWorld) );
			m_isNeedUpdateWorld = false;
		}

		HRESULT hr;
		if (NULL!=m_pCurGpuPro)
		{
			if ( op->useIndexes )
			{
				if (m_pIndexBuffer!=op->indexData->indexBuffer)
				{
					m_pIndexBuffer = op->indexData->indexBuffer;
					hr = m_pD3DDevice->SetIndices( ((DX9IndexHardwareBuffer*)op->indexData->indexBuffer)->getD3DIndexBuffer() );
				}
				do
				{
					hr = m_pD3DDevice->DrawIndexedPrimitive(
						(D3DPRIMITIVETYPE)op->operationType,
						static_cast<INT>(op->vertexData->vertexStart),
						0,
						static_cast<uint>(op->vertexData->vertexCount),
						static_cast<uint>(op->indexData->indexStart),
						static_cast<uint>(m_sRenderVal));

				}
				while (m_pCurGpuPro->_next());
			}
			else
			{
				do
				{
					hr = m_pD3DDevice->DrawPrimitive(
						(D3DPRIMITIVETYPE)op->operationType,
						static_cast<uint>(op->vertexData->vertexStart),
						static_cast<uint>(m_sRenderVal));
				}
				while (m_pCurGpuPro->_next());
			}
		}
		else
		{
			if ( op->useIndexes )
			{
				if (m_pIndexBuffer!=op->indexData->indexBuffer)
				{
					m_pIndexBuffer = op->indexData->indexBuffer;
					hr = m_pD3DDevice->SetIndices( ((DX9IndexHardwareBuffer*)op->indexData->indexBuffer)->getD3DIndexBuffer() );
				}

				hr = m_pD3DDevice->DrawIndexedPrimitive(
					(D3DPRIMITIVETYPE)op->operationType,
					static_cast<INT>(op->vertexData->vertexStart),
					0,
					static_cast<uint>(op->vertexData->vertexCount),
					static_cast<uint>(op->indexData->indexStart),
					static_cast<uint>(m_sRenderVal));
			}
			else
			{
				hr = m_pD3DDevice->DrawPrimitive(
					(D3DPRIMITIVETYPE)op->operationType,
					static_cast<uint>(op->vertexData->vertexStart),
					static_cast<uint>(m_sRenderVal));
			}
		}
		return SUCCEEDED(hr);
	}

	//static bool isNotDumped = true;
	bool DX9RenderPipeline::Begin(void)
	{
		m_sFaceCount = 0;
		m_sVertexCount = 0;
		m_sDrawPrimitiveCount = 0;
		HRESULT hr;

		SQR_TRY
		{
			//if(isNotDumped)EnableESHDump(isNotDumped);
			hr = m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
			//if(isNotDumped)EnableESHDump(false);]
			if(FAILED(hr))
				return false;
		}
		SQR_CATCH(exp)
		{
			string tmpErr;
			format(tmpErr,"_FC:%d",CGraphic::GetInst()->GetFrameCount());
			exp.AppendType("Present");
			exp.AppendMsg(tmpErr.c_str());
			GfkLogErrOnce(exp.ErrorTitle(),exp.ErrorMsg());	
			return false;
		}
		SQR_TRY_END


		hr = m_pD3DDevice->BeginScene();
		if(FAILED(hr))
			return false;

		m_pD3DDevice->SetVertexShaderConstantF(ACT_WORLD_MATRIX,(float*)&m_CurWorld,4);
		m_pD3DDevice->SetTransform( D3DTS_WORLD, &get(m_CurWorld) );
		return true;
	}

	void DX9RenderPipeline::_Flush(void)
	{
		m_pQuery->Issue(D3DISSUE_END);
		m_pQuery->GetData(NULL, 0, D3DGETDATA_FLUSH );
	}

	void DX9RenderPipeline::End(void)
	{

		CParamsBufferMgr::GetInst()->reset();
		m_pGpuMgr->unbind();	
		m_pD3DDevice->EndScene();

	}

	void DX9RenderPipeline::InitPipeline(void)
	{
		m_pGpuMgr = CGpuProgramMgr::GetInst();
		m_pIndexBuffer = NULL;
		m_pBinding = NULL;
		m_pVectexDec = NULL;
		m_pD3DDevice = GetD3DDevice();
		m_pD3DDevice->CreateQuery( D3DQUERYTYPE_EVENT, &m_pQuery );
		m_pD3DDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pQueryOcclusion );
		SetRenderStyle(STATIC_RS());
		SetAmbientSrcAsMaterial(true);
		ResetLightAttribute();
	}

	void DX9RenderPipeline::ReleasePipeline(void)
	{
		SafeRelease(m_pQuery);
		SafeRelease(m_pQueryOcclusion);
		//m_GroupMgr.Release();
	}

	void DX9RenderPipeline::_setVertexDeclaration(CVertexDeclaration* decl)
	{
		if (decl==m_pVectexDec)
			return;
		m_pVectexDec = decl;
		m_pVectexDec->update();
		m_pD3DDevice->SetVertexDeclaration( ((DX9VertexDeclaration*)m_pVectexDec)->getD3DVertexDecl());
	}

	void DX9RenderPipeline::_setVertexBufferBinding(CVertexBufferBinding* binding)
	{
		AstMsg(m_pD3DDevice, "_setVertexBufferBinding使用错误,必须在begin和end之间使用");

		m_pBinding = binding;
		if(NULL == m_pBinding)
		{
			m_pMainBuffer = NULL;
			m_pD3DDevice->SetStreamSource(0,NULL, 0, 0);
			return;
		}

		CVertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend;
		const CVertexBufferBinding::VertexBufferBindingMap& binds = binding->GetBindings();
		iend = binds.end();
		size_t source = 0;

		i = binds.begin();
		if( i->second.m_pBuffer == m_pMainBuffer && !binding->NeedUpdate())
		{
			++i;
			++source;
		}
		else
		{
			m_pMainBuffer = i->second.m_pBuffer;
		}

		for (; i != iend; ++i, ++source)
		{
			for ( ; source < i->first; ++source)
				m_pD3DDevice->SetStreamSource(static_cast<uint>(source), NULL, 0, 0);

			
			DX9VertexHardwareBuffer* d3d9buf =  (DX9VertexHardwareBuffer*)i->second.m_pBuffer;
			
			//m_pD3DDevice->SetStreamSourceFreq(static_cast<uint>(source),1);

			m_pD3DDevice->SetStreamSource(
				static_cast<uint>(source),
				d3d9buf->getD3D9VertexBuffer(),
				static_cast<uint>(i->second.m_sOffset),
				static_cast<uint>(i->second.m_sVertexSize));
		}
		m_pBinding->Update();
	}
	//------------------------------------------------
	void DX9RenderPipeline::SetViewport( CIRect& Rect )
	{
		D3DVIEWPORT9 vp;
		vp.X		= Rect.left;
		vp.Y		= Rect.top;
		vp.Width	= max(Rect.Width(),1);
		vp.Height	= max(Rect.Height(),1);
		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;
		m_pD3DDevice->SetViewport( &vp );
	}

	//------------------------------------------------------------------------------
	void
	DX9RenderPipeline::ClearDepth( float depth /*= 1.0f*/ )
	{
		m_pD3DDevice->Clear( 0, NULL, (DWORD)CMT_CLEAR_ZBUFFER_STENCI, 0, depth, 0 );
	}

	void DX9RenderPipeline::ClearRender( const CColor& color )
	{
		m_pD3DDevice->Clear( 0, NULL, (DWORD)CMT_CLEAR_TARGET, color, 1.0f, 0 );
	}

	void DX9RenderPipeline::ClearTarget( CRenderTarget* pTarget )
	{
		m_pD3DDevice->Clear( 0, NULL, (DWORD)pTarget->GetClearModeType(), pTarget->GetClearColor(), 1.0f, 0 );
	}

	void DX9RenderPipeline::UpdateFog()
	{
		if (m_FogMode == FMT_NONE)
			m_FogEnable  = false;

		m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, (DWORD)m_FogEnable );
		m_pD3DDevice->SetVertexShaderConstantF(ACT_FOG_ENABLE,(float*)&m_FogEnable,1);
		m_pD3DDevice->SetPixelShaderConstantF(ACT_FOG_ENABLE,(float*)&m_FogEnable,1);
		if (m_FogEnable)
		{
			m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOGMODE(m_FogMode));
			m_pD3DDevice->SetRenderState( D3DRS_FOGCOLOR, (DWORD)m_FogColor );
			switch (m_FogMode)
			{
			case FMT_EXP:
			case FMT_EXP2:
				m_pD3DDevice->SetRenderState( D3DRS_FOGDENSITY ,  *(DWORD*)&m_fFogDensity);
				break;
			case FMT_LINEAR:
				m_pD3DDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD*)&m_fFogSatrt );
				m_pD3DDevice->SetRenderState( D3DRS_FOGEND, *(DWORD*)&m_fFogEnd );

			}
		}
	}
	void DX9RenderPipeline::SetZDelta( float Z )
	{
		if ( m_fZDelta!=Z )
		{
			m_fZDelta = Z;
			m_pD3DDevice->SetRenderState( D3DRS_DEPTHBIAS,*(DWORD*)&m_fZDelta );
		}
	}

	void DX9RenderPipeline::SetAmbient( uint32 Color )
	{
		if ( m_Ambient != Color )
		{
			m_Ambient = Color;

			D3DXCOLOR lColor = Color;
			m_pD3DDevice->SetRenderState( D3DRS_AMBIENT, Color );
			m_pD3DDevice->SetVertexShaderConstantF(ACT_GLOBAL_AMBIENT,lColor,1);
		}
	}

	void DX9RenderPipeline::ResetLightAttribute()
	{
		D3DXCOLOR lColor = m_Ambient;

		m_pD3DDevice->SetRenderState( D3DRS_AMBIENT, m_Ambient );
		m_pD3DDevice->SetVertexShaderConstantF(ACT_GLOBAL_AMBIENT,lColor,1);
	}

	void DX9RenderPipeline::SetAmbientSrcAsMaterial( bool bMaterial )
	{
		m_pD3DDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE,(DWORD)bMaterial );
	}

	void DX9RenderPipeline::SetTextureMatrixImmediate( uint32 uTextStage, const CMatrix* pMatrix )
	{
		if (NULL == pMatrix)
		{
			m_bTexMatEnable[uTextStage] = false;
			m_pD3DDevice->SetTextureStageState( uTextStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			return;
		}
		if ( m_bTexMatEnable[uTextStage] )
		{
			//有效的输入矩阵
			if ( *(D3DXMATRIX*)pMatrix != *(D3DXMATRIX*)&m_TextureMatrix[uTextStage] )
				m_pD3DDevice->SetTransform( D3DTRANSFORMSTATETYPE( D3DTS_TEXTURE0 + uTextStage), (D3DXMATRIX*)pMatrix );
		}
		//当前开关关闭
		else
		{
			m_bTexMatEnable[uTextStage] = true;
			if ( *(D3DXMATRIX*)pMatrix == *(D3DXMATRIX*)&m_TextureMatrix[uTextStage] )
				m_pD3DDevice->SetTextureStageState( uTextStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
			else
			{
				m_pD3DDevice->SetTransform( D3DTRANSFORMSTATETYPE( D3DTS_TEXTURE0 + uTextStage), (D3DXMATRIX*)pMatrix );
				m_pD3DDevice->SetTextureStageState( uTextStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
			}
		}
	}

	//------------------------------------------------------------------------------
	void
	DX9RenderPipeline::SetLight( index_t lightIndex,const LightStruct* light )
	{
		Ast(lightIndex < 8);
		if (NULL != light)
		{
			m_pD3DDevice->LightEnable(lightIndex, TRUE);
			m_pD3DDevice->SetLight(lightIndex, (D3DLIGHT*)light);
		}
		else
		{
			m_pD3DDevice->LightEnable(lightIndex, FALSE);
		}
	}

	//------------------------------------------------------------------
	DWORD DX9RenderPipeline::get(Usage usage)
	{
		DWORD D3DUsage= 0;
		if (usage & HBU_DYNAMIC)
			D3DUsage |= D3DUSAGE_DYNAMIC;
		if (usage & HBU_WRITE_ONLY)
			D3DUsage |= D3DUSAGE_WRITEONLY;
		return D3DUsage;
	}

	DWORD DX9RenderPipeline::get(LockOptions options, Usage usage)
	{
		DWORD D3DLock = 0;
		if (options == HBL_DISCARD)
			if (usage & HBU_DYNAMIC)
				D3DLock |= D3DLOCK_DISCARD;

		if (options == HBL_READ_ONLY)
			if (!(usage & HBU_WRITE_ONLY))
				D3DLock |= D3DLOCK_READONLY;

		if (options == HBL_NO_OVERWRITE)
			if (usage & HBU_DYNAMIC)
				D3DLock |= D3DLOCK_NOOVERWRITE;
		return D3DLock;
	}

	D3DDECLTYPE DX9RenderPipeline::get(CVertexElementType vType)
	{
		switch (vType)
		{
		case VET_COLOUR:
			return D3DDECLTYPE_D3DCOLOR;
		case VET_FLOAT1:
			return D3DDECLTYPE_FLOAT1;
		case VET_FLOAT2:
			return D3DDECLTYPE_FLOAT2;
		case VET_FLOAT3:
			return D3DDECLTYPE_FLOAT3;
		case VET_FLOAT4:
			return D3DDECLTYPE_FLOAT4;
		case VET_SHORT2:
			return D3DDECLTYPE_SHORT2;
		case VET_SHORT4:
			return D3DDECLTYPE_SHORT4;
		case VET_UBYTE4:
			return D3DDECLTYPE_UBYTE4;
		}
		return D3DDECLTYPE_FLOAT3;
	}

	D3DDECLUSAGE DX9RenderPipeline::get(CVertexElementSemantic sem)
	{
		switch (sem)
		{
		case VES_BLEND_INDICES:
			return D3DDECLUSAGE_BLENDINDICES;
		case VES_BLEND_WEIGHTS:
			return D3DDECLUSAGE_BLENDWEIGHT;
		case VES_DIFFUSE:
		case VES_SPECULAR:
		case VES_COLOR:
			return D3DDECLUSAGE_COLOR; // 用index区分
		case VES_NORMAL:
			return D3DDECLUSAGE_NORMAL;
		case VES_POSITION:
			return D3DDECLUSAGE_POSITION;
		case VES_TEXTURE_COORDINATES:
			return D3DDECLUSAGE_TEXCOORD;
		case VES_BINORMAL:
			return D3DDECLUSAGE_BINORMAL;
		case VES_TANGENT:
			return D3DDECLUSAGE_TANGENT;
		case VES_POSITIONT:
			return D3DDECLUSAGE_POSITIONT;
		case VES_PSIZE:
			return D3DDECLUSAGE_PSIZE;
		}

		return D3DDECLUSAGE_POSITION;
	}

	D3DFORMAT DX9RenderPipeline::get(_texFORMAT format)
	{
		return type_cast<D3DFORMAT>(format);
	}

	D3DXMATRIX&	DX9RenderPipeline::get(const CMatrix& mat )
	{
		return type_cast<D3DXMATRIX>(mat);
	}

	D3DRENDERSTATETYPE& DX9RenderPipeline::get( const RENDERSTATETYPE& rst )
	{
		return type_cast<D3DRENDERSTATETYPE>(rst);
	}

	D3DSAMPLERSTATETYPE& DX9RenderPipeline::get( const SAMPLERSTATETYPE& sst )
	{
		return type_cast<D3DSAMPLERSTATETYPE>(sst);
	}

	D3DTEXTURESTAGESTATETYPE& DX9RenderPipeline::get( const TEXTURESTAGESTATETYPE& tst )
	{
		return type_cast<D3DTEXTURESTAGESTATETYPE>(tst);
	}

	D3DMATERIAL& DX9RenderPipeline::get( const sMaterial& material )
	{
		return type_cast<D3DMATERIAL>(material);
	}

	bool DX9RenderPipeline::boxProbe( const CVector3f& Min, const CVector3f& Max, const CVector3f& RayPosition, const CVector3f& RayDirection)
	{
		return TRUE==D3DXBoxBoundProbe( (D3DXVECTOR3*)&Min,(D3DXVECTOR3*)&Max,(D3DXVECTOR3*)&RayPosition,(D3DXVECTOR3*)&RayDirection );
	}

	//------------------------------------------------------------------------------
	CMatrix&
	DX9RenderPipeline::MatrixInverse( CMatrix& matrix )
	{
		return *(CMatrix*)D3DXMatrixInverse((D3DXMATRIX*)&matrix, NULL, (D3DXMATRIX*)&matrix);
	}
}

#include "stdafx.h"
#include "BaseHelper.h"
#include "CTexture.h"
#include "CSkyMgr.h"
#include "CGraphic.h"
#include "CVertexDeclaration.h"
#include "CRenderGroup.h"
#include "CRenderPipeline.h"
CIRect rt;

namespace sqr
{
	CSkyMgr::CSkyMgr(CRenderTarget* pTarget)
		//: CElementNode(this , 0xffffffff)
		: m_pSkyTexture(NULL)
		, m_pRenderTarget(pTarget)
	{
		InitSkyBox();
		m_pRenderTarget->GetGroup()->RegisterBatch( ID_SKYBOX_FILTER , this );
		//m_pParentManager = this;
		first = true;
		umove = 0.0f;
		preumove = 0.0f;
		vmove = 0.0f;
		prevmove = 0.0f;
	}

	CSkyMgr::~CSkyMgr()
	{
		if( m_pSkyTexture )
			SafeRelease(m_pSkyTexture);
		m_pRenderTarget->GetGroup()->UnRegisterBatch( ID_SKYBOX_FILTER , this );
		CHardwareBufferMgr::GetInst()->DestroyBuffer(m_RenderOP.vertexData->vertexBufferBinding->GetBuffer(0));

		m_RenderOP.Destroy();
		m_ElePool.clear();
	}

	void CSkyMgr::InitSkyBox()
	{
		AddRef(); 

		m_pRenderTarget->GetRect( rt );

		CVertexHardwareBuffer* _pVertexHBuffer	= CHardwareBufferMgr::GetInst()->createVertexBuffer(
			sizeof(Vertex2D), 4 ,HBU_WRITE_ONLY);

		Vertex2D *vertex = (Vertex2D*)_pVertexHBuffer->lock(HBL_DISCARD);

		float x1 = (float)rt.left;
		float y1 = (float)rt.top;
		float x2 = (float)rt.right;
		float y2 = (float)rt.bottom;

		float u1 = 0.0f;
		float v1 = 0.0f;
		float u2 = 1.0f;
		float v2 = 1.0f;

		vertex[0] = Vertex2D( x1, y2, MAX_DEPTH, 0xffffffff, u1, v2 ); //对点写数据
		vertex[1] = Vertex2D( x1, y1, MAX_DEPTH, 0xffffffff, u1, v1 );
		vertex[2] = Vertex2D( x2, y2, MAX_DEPTH, 0xffffffff, u2, v2 );
		vertex[3] = Vertex2D( x2, y1, MAX_DEPTH, 0xffffffff, u2, v1 );

		_pVertexHBuffer->unlock();

		CVertexDeclaration* pVD = NULL;
		pVD = CVertexDeclaration::GET_VERDECL(Vertex2D::Format);

		m_RenderOP.Create();
		m_RenderOP.indexData->indexStart = 0;
		m_RenderOP.indexData->indexCount = 0;
		m_RenderOP.indexData->indexBuffer = NULL;
		m_RenderOP.useIndexes = false;
		m_RenderOP.vertexData->vertexStart = 0;
		m_RenderOP.vertexData->vertexCount = 4;
		m_RenderOP.vertexData->vertexDeclaration = pVD;
		m_RenderOP.operationType = PT_TRIANGLESTRIP;

		m_RenderOP.vertexData->vertexBufferBinding->SetBinding(0, _pVertexHBuffer );
	}

	void CSkyMgr::Render()
	{
		if(m_pSkyTexture==NULL)
			return;

		if(m_isReset)
		{
			if(FAILED(CMainWindowTarget::GetInst()->IsRenderActive()))
				return;
			ResetSkyBoxWnd();
		}
		
		CRenderPipeline::GetInst()->Render(&m_RenderOP, &m_Param);
	}

	void CSkyMgr::SetUVOffset( const float u, const float v )
	{
		if(m_pSkyTexture==NULL)
			return;

		CVertexHardwareBuffer* _pVertexHBuffer = NULL;
		_pVertexHBuffer = m_RenderOP.vertexData->vertexBufferBinding->GetBuffer(0);

		Vertex2D *vertex = (Vertex2D*)_pVertexHBuffer->lock(HBL_DISCARD);

		float x1 = (float)rt.left;
		float y1 = (float)rt.top;
		float x2 = (float)rt.right;
		float y2 = (float)rt.bottom;


		float u1 = 0.0f + u;
		float v1 = 0.0f + v;
		float u2 = 1.0f + u;
		float v2 = 1.0f + v;

		vertex[0] = Vertex2D( x1, y2, MAX_DEPTH, 0xffffffff, u1, v2 ); //对点写数据
		vertex[1] = Vertex2D( x1, y1, MAX_DEPTH, 0xffffffff, u1, v1 );
		vertex[2] = Vertex2D( x2, y2, MAX_DEPTH, 0xffffffff, u2, v2 );
		vertex[3] = Vertex2D( x2, y1, MAX_DEPTH, 0xffffffff, u2, v1 );

		_pVertexHBuffer->unlock();
	}

	void CSkyMgr::SetSkyBoxTextureName( const string& name )
	{
		if(name.size() == 0)
			return;

		if ( FAILED(CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), name.c_str(), &m_pSkyTexture ) ) )
		{
			ostringstream strm;
			strm<<"Create SkyBox Texture failed with file:"<<name;
			ArtErr(strm.str());
		}

		this->m_Param.m_RS.m_Texture_S0		   = m_pSkyTexture;
		this->m_Param.m_RS.m_AlphaBlendEnable  = false;
		this->m_Param.m_RS.m_AlphaBlendEnable  = false;

		this->m_Param.m_RS.m_TexOP_S0   = TOP_SELECTARG1;
		this->m_Param.m_RS.m_TexArg1_S0 = TA_TEXTURE;
		m_isReset = true;
	}			

	void CSkyMgr::ResetSkyBoxWnd()
	{
		SQR_TRY
		{
			if(m_pSkyTexture==NULL)
				return;

			if(FAILED(CMainWindowTarget::GetInst()->IsRenderActive()))
			{
				m_isReset = true;
				return;
			}

			if ( !m_RenderOP.vertexData || !m_RenderOP.vertexData->vertexBufferBinding )
			{
				GfkLogErr("CSkyMgr::ResetSkyBoxWnd() failed.", "m_RenderOP未初始化");
				return;
			}

			CVertexHardwareBuffer* _pVertexHBuffer = m_RenderOP.vertexData->vertexBufferBinding->GetBuffer(0);
			if ( !_pVertexHBuffer )
			{
				GfkLogErr("CSkyMgr::ResetSkyBoxWnd() failed.", "getBuffer失败");
				return;
			}

			Vertex2D *vertex = (Vertex2D*)_pVertexHBuffer->lock(HBL_DISCARD);
			if ( vertex == 0 )
			{
				GfkLogErr("CSkyMgr::ResetSkyBoxWnd() failed.", "lock失败");
				return;
			}

			if ( !m_pRenderTarget )
			{
				_pVertexHBuffer->unlock();
				GfkLogErr("CSkyMgr::ResetSkyBoxWnd() failed.", "m_pRenderTarget == NULL");
				return;
			}
			m_pRenderTarget->GetRect( rt );

			float x1 = (float)rt.left;
			float y1 = (float)rt.top;
			float x2 = (float)rt.right;
			float y2 = (float)rt.bottom;

			float u1 = 0.0f;
			float v1 = 0.0f;
			float u2 = 1.0f;
			float v2 = 1.0f;

			vertex[0] = Vertex2D( x1, y2, MAX_DEPTH, 0xffffffff, u1, v2 ); //对点写数据
			vertex[1] = Vertex2D( x1, y1, MAX_DEPTH, 0xffffffff, u1, v1 );
			vertex[2] = Vertex2D( x2, y2, MAX_DEPTH, 0xffffffff, u2, v2 );
			vertex[3] = Vertex2D( x2, y1, MAX_DEPTH, 0xffffffff, u2, v1 );

			_pVertexHBuffer->unlock();
			m_isReset = false;
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("Sky Reset失败");
			GfkLogExp(exp);
		}
		SQR_TRY_END;

	}

	void CSkyMgr::SetCameraDest( const CVector3f& dest )
	{
		if ( m_bMove )
		{
			if ( first )
			{
				first = false;
				umove = 0.0f;
				vmove = 0.0f;
			}
			else
			{
				if ( dest == m_PreDest )
				{
					umove = preumove;
					vmove = prevmove;
				}
				else
				{
					if ( (dest.x - m_PreDest.x) > 0 )
						umove -= m_fBackRightMove;
					else
						umove += m_fBackLeftMove;

					if ( (dest.z - m_PreDest.z) > 0 )
						vmove -= m_fBackTopMove;
					else
						vmove += m_fBackBottomMove;
				}
			}

			SetUVOffset(umove, vmove);

			preumove = umove;
			prevmove = vmove;
		}
		m_PreDest = dest;
	}
}

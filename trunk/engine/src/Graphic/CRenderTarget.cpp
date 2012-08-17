#include "stdafx.h"
#include "CRenderTarget.h"
#include "CRenderPipeline.h"
#include "CRenderGroup.h"
#include "CGraphic.h"
#include "CRendererBase.h"
#include "CCamera.h"
#include "TSqrAllocator.inl"
#include "BaseHelper.h"
#include "CColor.inl"

CRenderTarget::CRenderTarget()
		: m_bIsAutoRender(false)
		, m_pRenderGroup(NULL)
		, m_fLockAsp(0.0f)
		, m_cmtClear(CMT_CLEAR_ALL)
		, m_nFarCount(0)
		, m_nNearCount(0)
		, m_nPostCount(0)
		, m_isAutoClearColor(true)
		, m_ClearColor( CColor::Black )
		, m_pBloomTarget(NULL)
{
	m_rcViewPort.left	= 0;
	m_rcViewPort.right	= 1;
	m_rcViewPort.top	= 0;
	m_rcViewPort.bottom	= 1;
}

CRenderTarget::~CRenderTarget()
{
	GfkSafeDelete(m_pBloomTarget);
	SafeRelease(m_pRenderGroup);
}

void CRenderTarget::Render(void)
{
	if(NULL == m_pRenderGroup)
		return;

	UpdateAspect();
	UpdateViewport();

	float inverseViewSize[2] = { 1.0f / m_rcViewPort.Width(), 1.0f / m_rcViewPort.Height() };
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_INVERSE_VIEW_SIZE, inverseViewSize, 1);
	m_pRenderGroup->GetCamera()->SetDeltaViewMatrix(m_matPreDeltaView);
	m_pRenderGroup->Render();


	if (NULL != m_pBloomTarget)
	{
		m_pBloomTarget->DoPostProcess();
		m_pRenderGroup->PostRender();
	}

	CRenderPipeline::GetInst()->ClearDepth();
	//Flip放到这,防止虑镜影响GUI
	Flip();

	for ( int i = 0; i < (int)m_lRenderSystems.size(); i++ )
	{
		SQR_TRY
		{	
			m_lRenderSystems[i]->Render();
		}
		SQR_CATCH(exp)
		{
			exp.AppendType("子渲染系统错误");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}
		
	m_pRenderGroup->Reset();
}

void	CRenderTarget::Clear()
{
	if(m_cmtClear!=CMT_CLEAR_NONE)
		CRenderPipeline::GetInst()->ClearTarget(this);
}

void	CRenderTarget::SetClearColor(const CColor& color)
{
	m_ClearColor = color;
}

CColor	CRenderTarget::GetClearColor(void)
{
	return m_isAutoClearColor&&NULL!=m_pRenderGroup?CColor(m_pRenderGroup->GetFogColor()):m_ClearColor;
}

void	CRenderTarget::AutoClearColor( bool bAuto )
{
	m_isAutoClearColor = bAuto;
}

void	CRenderTarget::UpdateViewport()
{
	CRenderPipeline::GetInst()->SetViewport(m_rcViewPort);
	CRenderPipeline::GetInst()->SetCamera(m_pRenderGroup->GetCamera());
}

CCamera*	 CRenderTarget::GetCamera(void)
{
	return m_pRenderGroup?m_pRenderGroup->GetCamera():NULL;
}

void CRenderTarget::UpdateAspect()
{
	if( m_rcViewPort.Height() != 0 )
		m_pRenderGroup->GetCamera()->setAspectRatio((float)m_rcViewPort.Width() / (float)m_rcViewPort.Height());
	else
		m_pRenderGroup->GetCamera()->setAspectRatio(m_fLockAsp);
}

bool CRenderTarget::Update(void)
{
	if (m_pRenderGroup==NULL)
		return false;

	if (isLockAspectRatio())
	{
		DWORD rW = m_rcViewPort.Width();
		DWORD rH = m_rcViewPort.Height();
		DWORD nW = rW;
		DWORD nH = rH;

		if (m_fLockAsp*rH > rW)
			nH = (DWORD)(rW/m_fLockAsp+0.5f);
		else
			nW = (DWORD)(rH*m_fLockAsp+0.5f);

		m_rcViewPort.left = (DWORD)((rW - nW)*0.5f);
		m_rcViewPort.top = (DWORD)((rH - nH)*0.5f);
		m_rcViewPort.right = m_rcViewPort.left + nW;
		m_rcViewPort.bottom = m_rcViewPort.top + nH;
	}

	if(m_pBloomTarget)
		m_pBloomTarget->InitParams();

	return true;
}

void CRenderTarget::SetRenderGroup(CRenderGroup* pRenderGroup)
{
	if ( pRenderGroup!=m_pRenderGroup )
	{
		SafeRelease(m_pRenderGroup);
		m_pRenderGroup = pRenderGroup;
		if(m_pRenderGroup)
			m_pRenderGroup->AddRef();
		Update();
	}
}

void CRenderTarget::InsertRenderer( CRendererBase* pRenderSys )
{
	m_lRenderSystems.push_back( pRenderSys );
}

void CRenderTarget::DeleteRenderer( CRendererBase* pRenderSys )
{
	for ( int i = 0; i < (int)m_lRenderSystems.size(); i++ )
	{
		if ( m_lRenderSystems[i] == pRenderSys )
		{
			m_lRenderSystems.erase( m_lRenderSystems.begin() + i );
			return;
		}
	}
}

void CRenderTarget::AddFilter( ITexture* pTexture, uint32 Color, bool bNear, bool bMulti, bool bratio, _TEXTUREADDRESS samp, CFRect* rt)
{
	Filter& CurFiler	= bNear ? m_NearFilter[m_nNearCount++] : m_FarFilter[m_nFarCount++];
	CurFiler.pText		= pTexture;
	CurFiler.dwColor	= Color;
	CurFiler.bMulti		= bMulti;
	CurFiler.bRatio		= bratio;
	CurFiler.eSamp		= samp;
	if( rt )
		CurFiler.cTexRt		= *rt;
}

void	CRenderTarget::AddPostFilter( CPostFilter* pPostFilter )
{
	if(pPostFilter==NULL)
		return;
	m_PostFilter[m_nPostCount++] = pPostFilter;
}

void CRenderTarget::AddFilterOff( float xDelta, float yDelta, float zDelta )
{
	m_matCurDeltaView._41 += xDelta;
	m_matCurDeltaView._42 += yDelta;
	m_matCurDeltaView._43 += zDelta;
}

void CRenderTarget::Flip()
{
	m_matPreDeltaView = m_matCurDeltaView;
	m_matCurDeltaView.Identity();

	CGraphic* pGph = CGraphic::GetInst();

	for ( int i = 0; i < m_nNearCount; i++ )
	{
		Vertex2D* pVB;
		RenderState* pRS;
		if( pGph->GetRenderStack( RS_DEFAULT, m_NearFilter[i].pText, NULL, PT_TRIANGLESTRIP, 4, 2,
			Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL, (void**)&pRS ) )
		{
			if ( m_NearFilter[i].bRatio )
			{
				pVB[0] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.bottom),	0.99999994f, m_NearFilter[i].dwColor, m_NearFilter[i].cTexRt.left, m_NearFilter[i].cTexRt.bottom );
				pVB[1] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.top),	0.99999994f, m_NearFilter[i].dwColor, m_NearFilter[i].cTexRt.left, m_NearFilter[i].cTexRt.top );
				pVB[2] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.bottom),0.99999994f, m_NearFilter[i].dwColor, m_NearFilter[i].cTexRt.right, m_NearFilter[i].cTexRt.bottom );
				pVB[3] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.top),	0.99999994f, m_NearFilter[i].dwColor, m_NearFilter[i].cTexRt.right, m_NearFilter[i].cTexRt.top );

				pRS->m_UAddr_S0 = m_NearFilter[i].eSamp;
				pRS->m_VAddr_S0 = m_NearFilter[i].eSamp;
			}
			else
			{
				pVB[0] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.bottom),	0.99999994f, m_NearFilter[i].dwColor, 0, 1 );
				pVB[1] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.top),	0.99999994f, m_NearFilter[i].dwColor, 0, 0 );
				pVB[2] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.bottom),0.99999994f, m_NearFilter[i].dwColor, 1, 1 );
				pVB[3] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.top),	0.99999994f, m_NearFilter[i].dwColor, 1, 0 );
			}
			if(m_NearFilter[i].pText == NULL)
			{
				pRS->m_TexOP_S0		= TOP_SELECTARG2;
				pRS->m_AlpOP_S0		= TOP_SELECTARG2;
			}
			else
				pRS->m_AlpOP_S0		= TOP_MODULATE;
			pRS->m_LightEnable = FALSE;
			if ( !m_NearFilter[i].bMulti )
				pRS->m_DestBlend = BLEND_ONE;
		}
	}
	m_nNearCount = 0;
	
	for ( int i = 0; i < m_nFarCount; i++ )
	{
		Vertex2D* pVB;
		RenderState* pRS;
		if( pGph->GetRenderStack( RS_DEFAULT, m_FarFilter[i].pText, NULL, PT_TRIANGLESTRIP, 4, 2,
			Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL, (void**)&pRS ) )
		{
			if ( m_FarFilter[i].bRatio )
			{
				pVB[0] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.bottom),	0.000018f, m_FarFilter[i].dwColor, m_FarFilter[i].cTexRt.left, m_FarFilter[i].cTexRt.bottom );
				pVB[1] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.top),	0.000018f, m_FarFilter[i].dwColor, m_FarFilter[i].cTexRt.left, m_FarFilter[i].cTexRt.top );
				pVB[2] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.bottom),0.000018f, m_FarFilter[i].dwColor, m_FarFilter[i].cTexRt.right, m_FarFilter[i].cTexRt.bottom );
				pVB[3] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.top),	0.000018f, m_FarFilter[i].dwColor, m_FarFilter[i].cTexRt.right, m_FarFilter[i].cTexRt.top );

				pRS->m_UAddr_S0 = m_FarFilter[i].eSamp;
				pRS->m_VAddr_S0 = m_FarFilter[i].eSamp;
			}
			else
			{
				pVB[0] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.bottom),	0.000018f, m_FarFilter[i].dwColor, 0, 1 );
				pVB[1] = Vertex2D( float(m_rcViewPort.left),float(m_rcViewPort.top),	0.000018f, m_FarFilter[i].dwColor, 0, 0 );
				pVB[2] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.bottom),0.000018f, m_FarFilter[i].dwColor, 1, 1 );
				pVB[3] = Vertex2D( float(m_rcViewPort.right),float(m_rcViewPort.top),	0.000018f, m_FarFilter[i].dwColor, 1, 0 );
			}
			if(m_FarFilter[i].pText == NULL)
			{
				pRS->m_TexOP_S0		= TOP_SELECTARG2;
				pRS->m_AlpOP_S0		= TOP_SELECTARG2;
			}
			else
				pRS->m_AlpOP_S0		= TOP_MODULATE;
			pRS->m_LightEnable	= FALSE;
			if ( !m_FarFilter[i].bMulti )
				pRS->m_DestBlend = BLEND_ONE;
		}
	}
	m_nFarCount = 0;

	for ( int i = 0; i < m_nPostCount; i++ )
	{
		m_PostFilter[i]->RenderFilter();
	}
	m_nPostCount = 0;
}


void CRenderTarget::SetWidth( uint32 width )
{
	width = max(width,1);
	m_rcViewPort.right = m_rcViewPort.left + width;
	Update();
}

void CRenderTarget::SetHeight( uint32 height )
{
	height = max(height,1);
	m_rcViewPort.bottom = m_rcViewPort.top + height;
	Update();
}

void CRenderTarget::SetRect( const CIRect& rect )
{
	m_rcViewPort = rect;
	m_rcViewPort.right	= max(m_rcViewPort.right,m_rcViewPort.left + 1);
	m_rcViewPort.bottom = max(m_rcViewPort.bottom,m_rcViewPort.top + 1);
	Update();
}

//------------------------------------------------------------------------------
CBloomTarget* CRenderTarget::GetTargetPipeline( void )
{
	if (CGpuProgramMgr::GetInst()->GetShaderVer() > Shader_V_1 && NULL==m_pBloomTarget)
		m_pBloomTarget = new CBloomTarget(this);
	return m_pBloomTarget;
}
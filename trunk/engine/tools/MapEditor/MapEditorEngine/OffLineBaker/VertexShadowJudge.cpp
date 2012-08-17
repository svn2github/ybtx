//------------------------------------------------------------------------------
//  VertexShadowJudge.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "VertexShadowJudge.h"
#include "CWindowTarget.h"
#include "Algorithm.h"
#include "CTexture.h"
#include "CGraphic.h"
const int DEPTH_TARGET_SIZE = 1024;

namespace sqr
{
	//------------------------------------------------------------------------------
	CVertexShadowJudge::CVertexShadowJudge()
	: m_isLocking(false)
	, m_pDepthData(NULL)
	{
		m_ShadowMatrix.Identity();
		// create a offscreen surface
		//Ast(GetD3DDevice()->CreateOffscreenPlainSurface(DEPTH_TARGET_SIZE,
		//	DEPTH_TARGET_SIZE, D3DFMT_R32F, D3DPOOL_SYSTEMMEM,
		//	m_pDepthTarget, NULL));
		CGraphic::GetInst()->CreateEmptyTexture( DEPTH_TARGET_SIZE, DEPTH_TARGET_SIZE, 1, TF_R32F, &m_pDepthTarget );
	}

	//------------------------------------------------------------------------------
	CVertexShadowJudge::~CVertexShadowJudge()
	{
		Ast(NULL == m_pDepthData);

		m_pDepthTarget = NULL;
	}

	//------------------------------------------------------------------------------
	bool
	CVertexShadowJudge::IsInShadow( const CVector3f& point )
	{
		Ast(m_isLocking);
		CVector4f projected;
		CVector4f t( point.x, point.y, point.z, 1.0f );
		projected = t * m_ShadowMatrix;
		float depth = this->tex2Dproj(projected) + 0.001f;// add a bias
		float z = projected.z / projected.w;
		
		return (z > depth);
	}

	//------------------------------------------------------------------------------
	void
	CVertexShadowJudge::Begin( CRenderTexture* target, const CMatrix& matrix )
	{
		m_ShadowMatrix = matrix;

		//ID3DTexturePtr texture = (IDirect3DTexture9*)(target->GetBuffer());
		//ID3DSurfacePtr surface;
		//Ast(texture->GetSurfaceLevel(0, surface.address()));

	/*	Ast(D3DXLoadSurfaceFromSurface(m_pDepthTarget, NULL, NULL, target,
									NULL, NULL, D3DX_FILTER_NONE, 0));*/

		m_pDepthTarget->FillFromTexture(target,NULL,NULL);
		_TEXLOCKED_RECT lockedRect;
		Ast(S_OK == m_pDepthTarget->Lock(0,&lockedRect, NULL, 0, LOCK_READONLY));
		Ast(lockedRect.Pitch == sizeof(float) * DEPTH_TARGET_SIZE);

		m_pDepthData = (float*)lockedRect.pBits;
		m_isLocking = true;
	}

	//------------------------------------------------------------------------------
	void
	CVertexShadowJudge::End()
	{
		Ast(S_OK == m_pDepthTarget->Unlock(0));
		m_isLocking = false;
		m_pDepthData = NULL;
	}

	//------------------------------------------------------------------------------
	inline float
	CVertexShadowJudge::tex2Dproj( const CVector4f& texCoord )
	{
		Ast(NULL != m_pDepthData);

		float u = texCoord.x / texCoord.w;
		float v = texCoord.y / texCoord.w;
		if (u < 0 || v < 0 || u > 1 || v > 1)
		{
			return 1;
		}

		int x = int(u * DEPTH_TARGET_SIZE);
		int y = int(v * DEPTH_TARGET_SIZE);
		float depth = 0;
		int index = y * DEPTH_TARGET_SIZE + x;
		index = clamp(index, 0, DEPTH_TARGET_SIZE * DEPTH_TARGET_SIZE - 1);

		return m_pDepthData[index];
	}
}
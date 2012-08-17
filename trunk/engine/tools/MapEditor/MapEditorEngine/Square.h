#pragma once
#include "Scene.h"
#include "CVector2.h"
#include "CVector3.h"

namespace sqr
{
	class ITexture;
	struct RenderState;

	class CSquare : public ISceneNode
	{
		//DWORD m_dwTextureHandle;
		float m_fSizeInWidth,m_fSizeInDepth;
		DWORD m_dwColor;

		// in strip
		CVector3f m_vPos[4];
		CVector2f m_vUV[4];
		DWORD m_dwClr[4];

		ITexture *m_pLockTexture;
		RenderState *m_pRS;
		string m_strLockTextureName;	// 锁定区域纹理名

	public:
		CSquare(ISceneNode * pParent, string texname, float fSizeInWidth, float fSizeInDepth, DWORD dwColor);
		~CSquare()
		{
			if ( m_pRS )
				m_pRS = NULL;
		}
		void Render();
		void SetDepth(float fDepth);
		void SetWidth(float fWidth);
	};
}
#pragma once
#include "Scene.h"

namespace sqr
{
	class CGraphic;

	class CWireBox : public ISceneNode
	{
		float m_fHeight;
		float m_fWidth;
		float m_fDepth;
		DWORD m_dwColor;

		// in line strip
		CVector3f m_vPos[5 * 6];
		DWORD m_dwClr[5 * 6];

	public:
		CWireBox(ISceneNode * pParent, bool bIsVisible);
		~CWireBox() { }
		void Create(float fHeight, float fWidth, float fDepth, DWORD dwColor);
		void Render(CGraphic * pCoreGraphic, bool bBefore, bool bRenderLine = false);
		void RenderBoxLine();
		void SetColor(DWORD);
		void IncreaseHeight();
		void DecreaseHeight();
		float GetHeight();
		void  SetHeight(float fHeight);
		float GetWidth();
		float GetDepth();
		void  SetWireBoxRange(float fHeight, float fWidth, float fDepth);
		void  UpdateWireBox();
	};
}
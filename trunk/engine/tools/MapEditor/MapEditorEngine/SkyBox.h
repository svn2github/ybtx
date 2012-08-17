#pragma once
#include "Scene.h"

namespace sqr
{
	class CSkyBox : public ISceneNode
	{
		enum ESKY_TYPE { EST_FRONT = 0, EST_BACK, EST_LEFT, EST_RIGHT, EST_TOP, EST_COUNT };

		CVector3f m_vPositions[20];
		CVector2f m_vUVs[20];
		DWORD m_dwTextureHandles[EST_COUNT];
		float m_fWidth;
		float m_fHeight;
		float m_fDepth;

	public:
		CSkyBox(ISceneNode * pParent);
		~CSkyBox();

		void Create(float fWidth, float fHeight, float fDepth, DWORD dwTextureHandleFront, DWORD dwTextureHandleBack, DWORD dwTextureHandleLeft, DWORD dwTextureHandleRight, DWORD dwTextureHandleTop, DWORD dwTexelCnt);
		void Render();
		void Update(InputEvent & event);
	};
}
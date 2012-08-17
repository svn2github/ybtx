#pragma once
#include "BluelyGUI.h"
#include "CVector3.h"

const int MAGICBALL_SIZE = 128;

namespace sqr
{
	class ITexture;
	struct RenderState;

	class CMagicBall : public CBlyWnd
	{
	private:
		struct SMagicBlock
		{
			SMagicBlock()
			{
				::memset(clrBranch,0x00,sizeof(D3DCOLOR)*MAGICBALL_SIZE*MAGICBALL_SIZE);
				rc.left = rc.top = 0;
				rc.right = rc.bottom = MAGICBALL_SIZE;
				dwSrcPitch = MAGICBALL_SIZE*sizeof(DWORD);
			}

			D3DCOLOR clrBranch[MAGICBALL_SIZE*MAGICBALL_SIZE];
			RECT rc;
			DWORD dwSrcPitch;
		};

		SMagicBlock m_clrDB;
		CVector3f m_vLight;
		ITexture *m_pITexture;
		RenderState *m_pRS;

	public:
		CMagicBall() : m_pRS(NULL), m_pITexture(NULL) {}
		~CMagicBall()
		{
			if (m_pITexture)
				m_pITexture = NULL;
			if (m_pRS)
				m_pRS = NULL;
		}

		virtual void Render();
		virtual void Update(const CMsg& msg);
		BOOL Create(const TCHAR *pszName,const RECT *pRect,DWORD dwStyle,CBlyWnd *pParent);
		CVector3f* getLightDir();
	};
}
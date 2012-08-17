#pragma once
#include "CRenderDialog.h"

class CCharacterFollower;

namespace sqr
{
	class CDirectionInfo : public CRenderDialog
	{
	public:
		CDirectionInfo();
		~CDirectionInfo();

		void	SetCharacter(CCharacterFollower* pCharacter);
		void	SetCharacterEntityID(uint32 uEntityID); 
		void	DrawWndBackground();
		void	OnPreRender( uint32 uCurTime, CCamera* pCamera  );
		void	Render(void){};

	private:
		uint32	m_uCharacterEntityID;
	};
}
#pragma once
#include "Scene.h"


namespace sqr
{
	class CGameGacCameraNode
	{
		friend class CGameGacCamera;
		CGameGacCamera	 *m_pCamera;
	private:
		CGameGacCameraNode( const CGameGacCameraNode & );
		CGameGacCameraNode( CGameGacCamera *pCam);
		CGameGacCameraNode& operator = ( const CGameGacCameraNode & );

		uint16  m_iX;
		uint16  m_iY;

	public:
		void OnEvent(const CMsg& msg);
		void Render();

	};
}
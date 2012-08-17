#pragma once
#include "CMetaRegion.h"
#include "CTimeCheckPoint.h"

namespace sqr
{

	class CRenderLight : public CSceneMallocObject
	{
		CTimeCheckPoint	m_Timer;
		CRegionLI		m_LightInfo;
		float			m_xPosition;
		float			m_yPosition;
		float			m_zPosition;
		float			m_uPreChangeTime;

	public:
		CRenderLight( CRegionLI&	LightInfo, float xPosition, float yPosition, float zPosition );
		~CRenderLight(void);

		int32		GetDist( CPos& posCenterGrid );
		void		Render();
	};


}
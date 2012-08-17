#pragma once
#include "CRenderLight.h"
#include "BaseHelper.h"
#
namespace sqr
{
	class CRenderRegion : public CSceneMallocObject
	{ 
		bool						m_bLoaded;
		SList<CRenderLight*>	m_listTerrianLight;
		
	public:
		CRenderRegion() : m_bLoaded(false) {};
		~CRenderRegion(void)
		{ 
			SList<CRenderLight*>::iterator it  = m_listTerrianLight.begin();
			SList<CRenderLight*>::iterator end = m_listTerrianLight.end();
			while( it != end )
			{
				if( *it )
					GfkSafeDelete(*it);
				++it;
			}
		};

		bool						IsLoaded()									{ return m_bLoaded; }
		void						SetLoaded( bool bLoaded )					{ m_bLoaded = bLoaded; }
		SList<CRenderLight*>&	GetLightList()								{ return m_listTerrianLight; }
	};


}
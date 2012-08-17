#pragma once 
#include "CMirrorBufferData.h"

namespace sqr
{
	class CCoreQuadScene;
	class CCoreQuadObject;

	class CMirrorQuadObject : public CMirrorBufferData
	{
	public:
		CCoreQuadObject* GetQuadObject() 
		{
			return m_pQuadObject;
		}

	protected:
		CMirrorQuadObject(uint32 uObjId, const CFPos& pos);
		~CMirrorQuadObject();

	protected:
		CCoreQuadObject*	m_pQuadObject;
	};

}

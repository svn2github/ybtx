#pragma once
#include "CLock.h"
#include "TQuadAllocator.h"

namespace sqr
{
	struct CAoiShape;

	class CAoiShapeCache
	{
	public:
		static CAoiShapeCache& Inst();

		CAoiShape* GetShape(const char* szAoiFile);
		
	private:
		CAoiShapeCache(void);
		~CAoiShapeCache(void);

		typedef TQuadAllocator<pair<string, CAoiShape*> >	ShapeMapAllocator_t;
		typedef map<string,CAoiShape*,less<string>,ShapeMapAllocator_t>	ShapeMap_t;


		CLock		m_lkAoiShapes;
		ShapeMap_t	m_mapAoiShapes;
	};
}

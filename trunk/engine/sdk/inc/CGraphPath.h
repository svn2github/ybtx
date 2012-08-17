#pragma once


#include <list>
#include <map>
#include "CMathMallocObject.h"

namespace sqr
{
	class CGraphPath : public CMathMallocObject
	{
	public:
		typedef  MathList<int>           PathList;
		typedef  MathVector< PathList >  GraphaPath;
		CGraphPath();
		~CGraphPath();

		GraphaPath& GetPathMap();
		PathList& GetEndPath(int end);

	private:
		GraphaPath m_PathMap;
	};
}


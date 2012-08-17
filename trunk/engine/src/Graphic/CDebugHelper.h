#pragma once
#include "TSingleton.h"

namespace sqr
{
	class CDebugHelper : public Singleton<CDebugHelper>
	{
	public:
		CDebugHelper();
		~CDebugHelper();

		void EatMemory( uint32 nMB );
		void ReleaseMemory( uint32 nMB );

		void EatMemoryUntil( uint32 nMB );

	private:
		CDebugHelper( const CDebugHelper& );
		CDebugHelper& operator = ( const CDebugHelper& );

	private:
		typedef map<uint32, uint8*>		MemEatenMap;
		typedef MemEatenMap::iterator	MemEatenIter;
		
		MemEatenMap m_mapMemEaten;
	};
}
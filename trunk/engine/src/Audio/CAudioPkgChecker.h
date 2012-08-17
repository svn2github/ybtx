#pragma once
#include "TSingleton.h"

namespace sqr
{
	class CWinPkgFile;

	class CAudioPkgChecker : public Singleton<CAudioPkgChecker>
	{
	public:
		CAudioPkgChecker();
		~CAudioPkgChecker();

		uint32	GetCheckCode( const string& file );

	private:
		bool	readData( uint32& data, CWinPkgFile& pkgFile );

	private:
		typedef map<uint32, uint32> CheckMap;
		
		CheckMap	m_check_map;
		bool		m_bInvalid;
	};
}

#include "stdafx.h"
#include "CAudioPkgChecker.h"
#include "CPkgFile.h"
#include "PkgCommon.h"
#include "StringHelper.h"
#include "CWinPkgFile.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_AUD;

	static const string SOUNG_CHECK_FILE	= "sound/sound.crc";

	CAudioPkgChecker::CAudioPkgChecker()
		: m_bInvalid(false)
	{
		CWinPkgFile	pkgFile;
		if ( INVALID_HANDLE_VALUE == pkgFile.Open(PATH_ALIAS_AUD.c_str(), SOUNG_CHECK_FILE.c_str()) )
			goto READ_FAIL;

		uint32 file_count = 0;
		if ( !readData(file_count, pkgFile) )
			goto READ_FAIL;

		for ( uint32 i=0; i<file_count; ++i )
		{
			uint32 file_id  = 0;
			uint32 crc		= 0;

			if ( !readData(file_id, pkgFile) )
				goto READ_FAIL;
		
			if ( !readData(crc, pkgFile) )
				goto READ_FAIL;

			m_check_map.insert(make_pair(file_id, crc));
		}

		return;

	READ_FAIL:
		m_bInvalid = true;
	}

	CAudioPkgChecker::~CAudioPkgChecker()
	{
	}

	uint32 CAudioPkgChecker::GetCheckCode( const string& file )
	{
		if ( m_bInvalid )
			return 0;
	
		string audio_file = file;
		backslash_to_slash(audio_file);

		uint32 file_id = FileName2Id(audio_file.c_str());
		CheckMap::iterator it_find = m_check_map.find(file_id); 
		if ( it_find == m_check_map.end() )
			return 0;

		return it_find->second;
	}

	bool CAudioPkgChecker::readData( uint32& data, CWinPkgFile& pkgFile )
	{
		if ( pkgFile.Read((char*)&data, sizeof(uint32)) < 0 )
			return false;

		return true;
	}
}

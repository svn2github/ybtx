#pragma once

#include <hash_map>
#include <string>
#include <iostream>

#include "BaseTypes.h"

using namespace stdext;
using namespace std;

namespace sqr
{
	class CCrcChecking
	{
	public:
		CCrcChecking();
		~CCrcChecking();
		void ReadCrcCodeToMap();
		bool Check();
		bool DoCheck( const string& strFileName );

	private:
		const char* m_szCrcFileName;
		const unsigned int m_nMaxLen;
		hash_map<string, string> m_hmapCrcCode;
	};
}

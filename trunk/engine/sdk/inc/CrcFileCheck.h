#pragma once
#include "CrcAlgorithm.h"
#include "TSingleton.h"
namespace sqr
{
	class CrcFileCheck : public TSingleton<CrcFileCheck>
	{
	public:
		CrcFileCheck();
		~CrcFileCheck();
		uint32 DoCrc32Check( const wstring& strFile );
		void ReadCrcCodeToMap(const wstring& FileName );
		int64 GetFileCrcCode(string FileName){return m_mFile2CrcCode[FileName];}
	private:
		map<string,int64> m_mFile2CrcCode;
	};
}


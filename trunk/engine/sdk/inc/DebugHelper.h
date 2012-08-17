#pragma once
#include "CodeCvs.h"
#include "TSingleton.h"

#define U(s) utf8_to_utf16(s).c_str()

namespace sqr
{
	class CRefBufFile;
	class CPkgFile;

	class CCrc32 : public TSingleton<CCrc32>
	{
		friend class TSingleton<CCrc32>;
	public:
		void	InitCrc32(uint32& crc);
		void	MakeCrc32Sub(uint32& crc, const uint8* pData, uint32 len);
		void	MakeCrc32End(uint32& crc);

	private:
		CCrc32();

		uint32	m_Crc32Table[256];
	};

	void	ConsolePrint(const wchar_t *format, ...);

	bool	CheckPkgCrash( const CRefBufFile& bufFile );

	bool	CheckPkgCrash( CPkgFile& pkgFile );

	void	ShowErrReportBox( const string& szMsg );
}

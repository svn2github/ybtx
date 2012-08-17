#include "stdafx.h"
#include "DebugHelper.h"
#include "StringHelper.h"
#include "CBufFile.h"
#include "CPkgFile.h"
#include <stdarg.h>
#include "ErrLogHelper.h"

namespace sqr
{
	static bool g_bPkgCrashReported = false;

	void vConsolePrint(const wchar_t *format, va_list argptr)
	{
		wstring str;
		vformat(str, format, argptr);
		wprintf(str.c_str());
	}

	void ConsolePrint( const wchar_t *format, ... )
	{
		va_list argptr;
		va_start(argptr, format);
		sqr::vConsolePrint(format, argptr);
		va_end(argptr);
	}

	bool checkPkgCodeAndReport(uint32 file_code, uint32 pkg_code)
	{
		if ( g_bPkgCrashReported )
			return true;

		if ( pkg_code != 0 && pkg_code != file_code )
		{
			g_bPkgCrashReported = true;
			ShowErrReportBox("系统检测到您的客户端有数据损坏，请您重新安装以正常进行游戏。");
			CError exp("检测到包损坏");
			LogExp(exp);
			return true;
		}
		else
			return false;
	}

	bool CheckPkgCrash( const CRefBufFile& bufFile )
	{
		uint32 crc = 0;
		CCrc32::GetInst()->InitCrc32(crc);
		CCrc32::GetInst()->MakeCrc32Sub(crc, (const uint8*)bufFile.GetBuf(), bufFile.GetBufSize());
		CCrc32::GetInst()->MakeCrc32End(crc);
		return checkPkgCodeAndReport(crc, bufFile.GetCheckCode());
	}

	bool CheckPkgCrash( CPkgFile& pkgFile )
	{
		const int32 chunk_size = 128*1024;
		int32 curPos = pkgFile.Tell();
		SQR_TRY
		{
			pkgFile.Seek(0);

			char	buf[chunk_size] = {0};
			int32	leftSize = pkgFile.Size();
			uint32	crc		 = 0;
			CCrc32::GetInst()->InitCrc32(crc);
			while ( leftSize - chunk_size > 0 )
			{
				leftSize -= chunk_size;
				pkgFile.read(buf, chunk_size);
				CCrc32::GetInst()->MakeCrc32Sub(crc, (const uint8*)buf, chunk_size);
			}
			if ( leftSize > 0 )
			{
				pkgFile.read(buf, leftSize);
				CCrc32::GetInst()->MakeCrc32Sub(crc, (const uint8*)buf, leftSize);
			}
			CCrc32::GetInst()->MakeCrc32End(crc);

			pkgFile.Seek(curPos);

			return checkPkgCodeAndReport(crc, pkgFile.GetCheckCode());;
		}
		SQR_CATCH(exp)
		{
			pkgFile.Seek(curPos);
			string msg = format("pkgFile size:%u ", pkgFile.Size());
			exp.AppendType("图形组:CheckPkgCrash failed.");
			exp.AppendMsg(msg.c_str());
			LogExp(exp);
		}
		SQR_TRY_END

		return true;
	}

	void CCrc32::InitCrc32(uint32& crc)
	{
		crc = 0xffffffff;
	}

	void CCrc32::MakeCrc32Sub( uint32& crc, const uint8* pData, uint32 len )
	{
		for ( uint32 i=0; i<len; ++i )
		{
			crc = m_Crc32Table[(crc^pData[i]) & 0xff] ^ (crc >> 8);
		}
	}

	void CCrc32::MakeCrc32End(uint32& crc)
	{
		crc = ~crc;
	}

	CCrc32::CCrc32()
	{
		uint32 crc;
		for ( uint32 i=0; i<256; ++i )
		{
			crc = i;
			for ( uint8 j=0; j<8; ++j )
			{
				if ( crc & 1 )
					crc = (crc>>1) ^ 0xEDB88320; // crc32 多项式的值
				else
					crc >>= 1;
			}

			m_Crc32Table[i] = crc;
		}
	}

	void ShowErrReportBox( const string& szMsg )
	{
#ifdef _WIN32
		::MessageBox(NULL, szMsg.c_str(), "系统提示", MB_OK);
#endif
	}
}

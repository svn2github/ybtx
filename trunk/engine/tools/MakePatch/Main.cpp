#include "stdafx.h"
#include "MakePatch.h"
#include "FileProcess.h"
#include "IDiff.h"
#include "Memory.h"
#include "ExpHelper.h"
//#include "CodeCvs.h"
#include "CVerifyClient.h"

void PrintHelp()
{
	printf("usage: [options] <src> <dest>");
	printf("Available options are:");
	printf("  -t <type>     split with |, tga|tex|bmp");
	printf("  --c           non-compress patch data");
}

void NewHandler()
{
	CError exp("the call of new out of memory.");
	SQR_THROW(exp);
}



bool gbk_to_utf16( std::wstring& dest, const std::string& src )
{
#ifdef _WIN32
	dest.resize(src.size());
	const string old_locale = setlocale(LC_CTYPE, NULL);
	setlocale(LC_CTYPE, ".936");
	size_t unicodeLength = mbstowcs(const_cast<wchar_t*>(dest.data()), src.c_str(), src.size());
	setlocale(LC_CTYPE, old_locale.c_str());
	if (unicodeLength == -1)
	{
		dest = L"";
		return false;
	}
	dest.resize(unicodeLength);
#else
	string strTmp;
	if (!gbk_to_utf8(strTmp, src))
		return false;
	utf8_to_utf16(dest, strTmp);
	if (dest==L"")
	{
		return false;
	}
#endif
	return true;
}

std::wstring gbk_to_utf16(const std::string& src)
{
	wstring strTemp;
	gbk_to_utf16(strTemp,src);
	return strTemp;
}


void GenMd5CodeListFile(string sNewDir)
{
	wstring wsNewDir = gbk_to_utf16(sNewDir);
	wstring wsFileName = wsNewDir + L"\\ver.sys";

	CVerifyClient vc;

	vc.MakeVerificationFile(wsNewDir, wsFileName);
	
	if ( !vc.CheckIntegrity(wsFileName) )
	{
		wprintf(L"FAILED! 生成客户端校验文件时出错--校验文件ver.sys完整性没能过。严重\n");
		exit(1);
	}
	if ( !vc.VerifyDirectory( NULL,wsNewDir, wsFileName ) )
	{
		wprintf(L"FAILED! 文件模拟校验测试时出错--。在利用%s校验目录%s时发现不匹配。严重\n", wsFileName.c_str(), wsNewDir.c_str());
		wstring wsErr = vc.FetchErrorMsg();
		wprintf(L"%s\n", wsErr.c_str());
		exit(1);
	}
	wprintf(L"make verification file ver.sys success!\n");

}

// 需要三个参数，新版本根路径、旧版本根路径、生成Patch文件路径
int _tmain(int argc, _TCHAR* argv[])
{
	SQR_TRY
	{
		_set_se_translator(TransESHFun);
		set_new_handler(NewHandler);

		MakePatch patch;

		// 处理命令行参数
		int i = 0;
		for( i = 1; argv[i]; ++i )
		{
			if ( *argv[i] != '-' )
				break;

			if( !strcmp( argv[i], "-t" ) )
				patch.SetType(argv[++i]);
			else if ( !strcmp( argv[i], "--c" ) )
				patch.SetCompress(false);
		}
		// 生成md5校验列表文件
		GenMd5CodeListFile(argv[i]);

		// 根据update文件生成patch文件名
		patch.GetPatchVersion(argv[i]);
		patch.SetPatchName(argv[i+2]);

		// 搜索需要缺少或被需要改的文件
		patch.GetPatchFile(argv[i], argv[i+1]);
		patch.FetchPatchData();
		patch.GeneratePatch();
	}
	SQR_CATCH(exp)
	{
		printf("creat patch failed: %s\n", exp.ErrorTitle());
	}
	SQR_TRY_END

	return 0;
}





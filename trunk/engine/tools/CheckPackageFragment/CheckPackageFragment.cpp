#include "stdafx.h"
#include "CPathMgr.h"
#include "StringHelper.h"
#include "CodeCvs.h"
#include "ExpHelper.h"
#include "PkgCommon.h"

const uint32	OPEN_FILE_SUCCEED			= 0;
const string	ERR_PARAMS_INVALID			= "params invalid";
const string	ERR_CAN_NOT_OPEN_PACKAGE	= "can not open package";
const string	ERR_OPEN_FILE_FAILED		= "open file failed";
const wstring	OUT_FILE					= L"PackageFragments.txt";
const uint32	ONE_MB						= 1024*1024;
const uint32	ONE_KB						= 1024;

enum
{
	KB,
	MB,
	UNITS_COUNT
};
const wchar_t*  KB_MB[UNITS_COUNT] = {L" KB", L" MB"};

typedef vector<PackFileIndex>		PackFileList;

struct PackageInfo
{
	PackageInfo()
		: uPackSize(0)
	{
	}

	uint32			uPackSize;
	PackFileList	fileList;
};

typedef map<wstring, PackageInfo>	PackInfoList;

PackInfoList	g_PackList;
wstring			g_PackPath;
wstring			g_OutPath;


void		ParseParams( int argc, _TCHAR* argv[] );
FTW_RESULT	FindAllPackages( const wchar_t* szCurFile, const struct _stat* pFileInfo, FTW_FLAG fileType, void* pContext );
void		CollectPackagesInfo();
void		GenerateFragmentsInfo();
void		ProcessError( CError& err );


int _tmain(int argc, _TCHAR* argv[])
{
	SQR_TRY
	{
		ParseParams(argc, argv);
		FileTreeWalk(g_PackPath.c_str(), &FindAllPackages, NULL);
		CollectPackagesInfo();
		GenerateFragmentsInfo();
	}
	SQR_CATCH(exp)
	{
		ProcessError(exp);
	}
	SQR_TRY_END	

		return 0;
}


void ParseParams( int argc, _TCHAR* argv[] )
{
	if ( argc != 3 )
	{
		stringstream ss;
		ss << "usage: " << argv[0] << " [package path] [output path]\n";
		GenErr(ERR_PARAMS_INVALID, ss.str());
	}

	g_PackPath = argv[1];
	g_OutPath  = argv[2];

	MakeDirectory(g_OutPath.c_str());
	g_OutPath += L"/" + OUT_FILE;
}

FTW_RESULT FindAllPackages( const wchar_t* szCurFile, const struct _stat* pFileInfo, FTW_FLAG fileType, void* pContext )
{
	wstring szFileIn(szCurFile);
	tolower(szFileIn);
	backslash_to_slash(szFileIn);
	if ( fileType == _FTW_DIR || szFileIn.find(L".svn") != wstring::npos || szFileIn.find(L".pkg") == wstring::npos )
		return _FTW_CONTINUNE;

	g_PackList.insert(make_pair(szFileIn, PackageInfo()));
	return _FTW_CONTINUNE;
}

void CollectPackagesInfo()
{
	PackInfoList::iterator it  = g_PackList.begin();
	PackInfoList::iterator eit = g_PackList.end();

	for ( ; it != eit; ++it )
	{
		wstring			szCurPack	= it->first;
		PackageInfo&	packInfo	= it->second;

		FILE* fp = 0;
		if ( _wfopen_s(&fp, szCurPack.c_str(), L"rb") != OPEN_FILE_SUCCEED )
		{
			string errPack;
			utf16_to_gbk(errPack, szCurPack);
			GenErr(ERR_CAN_NOT_OPEN_PACKAGE, errPack);
		}

		PackageHead		curPackHead;
		PackFileIndex	curHeadIndex;
		ReadPackageHeadInfo(curPackHead, curHeadIndex, fp);
		SeekToFileIndexList(fp, curPackHead);
		packInfo.fileList.resize(curPackHead.number);
		for ( int32 i=0; i<curPackHead.number; ++i )
			ReadPackageFileIndex(packInfo.fileList[i], curPackHead, fp);
		
		fseek(fp, 0, SEEK_END);
		packInfo.uPackSize = ftell(fp);

		fclose(fp);
	}
}

void GenerateFragmentsInfo()
{
	FILE* pFileOut = 0;
	if ( _wfopen_s(&pFileOut, g_OutPath.c_str(), L"wb") != OPEN_FILE_SUCCEED )
	{
		string errFile;
		utf16_to_gbk(errFile, g_OutPath);
		GenErr(ERR_OPEN_FILE_FAILED, errFile);
	}

	uint8 fragUnit = KB;
	uint8 packUnit = KB;
	uint32 uTotalFragCount = 0;
	uint32 uTotalFragSize  = 0;
	uint32 uTotalPackSize  = 0;
	PackInfoList::iterator it_pack		= g_PackList.begin();
	PackInfoList::iterator it_pack_end	= g_PackList.end();
	wstringstream outContent;
	outContent.setf(ios::right, ios::adjustfield);
	for ( ; it_pack != it_pack_end; ++it_pack )
	{
		wstring szCurPack = it_pack->first;
		const PackageInfo& packInfo = it_pack->second;

		uint32 uFragCount = 0;
		uint32 uFragSize  = 0;
		for ( size_t iCurFile = 0; iCurFile < packInfo.fileList.size(); ++iCurFile )
		{
			if ( packInfo.fileList[iCurFile].space > 0 )
			{
				++uFragCount;
				uFragSize += packInfo.fileList[iCurFile].space;
			}
		}

		uTotalFragCount += uFragCount;
		uTotalFragSize  += uFragSize;
		uTotalPackSize	+= packInfo.uPackSize;
		
		fragUnit = KB;
		packUnit = KB;
		uint32 uFragDispSize = uFragSize / ONE_KB;
		uint32 uPackDispSize = packInfo.uPackSize / ONE_KB;
		if ( uFragDispSize > ONE_KB )
		{
			fragUnit = MB;
			uFragDispSize /= ONE_KB;
		}
		if ( uPackDispSize > ONE_KB )
		{
			packUnit = MB;
			uPackDispSize /= ONE_KB;
		}

		outContent << "package : " << szCurPack << "\n";
		outContent << "fragments count : " << setw(9) << uFragCount	   << "\n";
		outContent << "fragments size  : " << setw(6) << uFragDispSize << KB_MB[fragUnit] << "\n";
		outContent << "package   size  : " << setw(6) << uPackDispSize << KB_MB[packUnit];
		outContent << "\n\n";
	}

	fragUnit = KB;
	packUnit = KB;
	uTotalFragSize /= ONE_KB;
	uTotalPackSize /= ONE_KB;
	if ( uTotalFragSize > ONE_KB )
	{
		fragUnit = MB;
		uTotalFragSize /= ONE_KB;
	}
	if ( uTotalPackSize > ONE_KB )
	{
		packUnit = MB;
		uTotalPackSize /= ONE_KB;
	}
	outContent << "------------------------------------------------------" << "\n\n";
	outContent << "total fragments count : " << setw(9) << uTotalFragCount << "\n";
	outContent << "total fragments size  : " << setw(6) << uTotalFragSize  << KB_MB[fragUnit] << "\n";
	outContent << "total package   size  : " << setw(6) << uTotalPackSize  << KB_MB[packUnit];

	string outString;
	utf16_to_gbk(outString, outContent.str());
	fputs(outString.c_str(), pFileOut);

	fclose(pFileOut);
}

void ProcessError( CError& err )
{
	string szErrType = err.ErrorTitle();
	if ( szErrType == string(ERR_PARAMS_INVALID) )
		cerr << err.ErrorMsg();
}


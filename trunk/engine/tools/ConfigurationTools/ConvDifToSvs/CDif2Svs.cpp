#include "stdafx.h"
#include "CDif2Svs.h"
#include "CDifFile.h"
#include "CSvsFile.h"
#include "CSplitInfo.h"

uint32 CDif2Svs::uCount = 0;

void CDif2Svs::Convert(string& sDifFile, string& sSvsFile)
{
	CDifFile aDifFile(sDifFile);
	CSvsFile aSvsFile;
	aSvsFile.SetName(sSvsFile);
	size_t uRowCount = aDifFile.GetRowCount();
	aSvsFile.AddRowColPos(uRowCount - 1, 0);
	for(size_t i = 0; i < uRowCount; ++i)
	{
		size_t uColCount = aDifFile.GetColCountByRow(i);
		string sTDValue;
		aSvsFile.AddRowColPos(i, uColCount - 1);
		for(size_t j = 0; j < uColCount; ++j)
		{
			bool bIsNumber = aDifFile.BeNumber(i, j);
			sTDValue = aDifFile.GetDataByRowCol(i, j);
			aSvsFile.AddData_toTab(sTDValue, i, j, bIsNumber);
		}
	}
	aSvsFile.SaveFile();
}

FTW_RESULT CDif2Svs::Convert(const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	wstring strPostfix = L".dif"; //CSplitInfo::GetInst()->WGetLuaCfgExtend();
	wstring wstrCpp = L".ndf"; // CSplitInfo::GetInst()->WGetCppCfgExtend();

	//wstring wstrSpcFile = L"ChallengeLimitScene_Server.dif";
	//if( _wcsicmp( szFileName + nLen - wstrSpcFile.length(), wstrSpcFile.c_str() ) )
	//{
	//	return _FTW_CONTINUNE;
	//}

	if( !_wcsicmp( szFileName + nLen - strPostfix.length(), strPostfix.c_str() ) )
	{
		string sSrcFileName = utf16_to_gbk(szFileName);
		string sTgtFileName = sSrcFileName.substr(0, sSrcFileName.size() - 4) + ".svs";
		cout << "Dif2Svs:[" << (++uCount) << "]" << sSrcFileName << endl;
		Convert(sSrcFileName, sTgtFileName);
	}
	else if( !_wcsicmp( szFileName + nLen - wstrCpp.length(), wstrCpp.c_str() ) )
	{
		string sSrcFileName = utf16_to_gbk(szFileName);
		string sTgtFileName = sSrcFileName.substr(0, sSrcFileName.size() - 4) + ".svs";
		cout << "Ndf2Svs:[" << (++uCount) << "]" << sSrcFileName << endl;
		Convert(sSrcFileName, sTgtFileName);
	}
	return _FTW_CONTINUNE;
}

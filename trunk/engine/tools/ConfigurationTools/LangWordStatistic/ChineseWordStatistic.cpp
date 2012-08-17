#include "stdafx.h"
#include "ChineseWordStatistic.h"
#include "common.h"
#include "COriFileOperate.h"

int32 CChineseWordStatistic::s_nCount = 0;
ofstream CChineseWordStatistic::s_ofsWordCount1 = ofstream("F:/WordCount1/TotalWord1.txt");
ofstream CChineseWordStatistic::s_ofsWordCount2 = ofstream("F:/WordCount1/TotalWord2.txt");
ofstream CChineseWordStatistic::s_ofsWordCount3 = ofstream("F:/WordCount1/TotalWord3.txt");
hash_set<string> CChineseWordStatistic::s_hsetWordCount2 = hash_set<string>();
hash_set<string> CChineseWordStatistic::s_hsetWordCount3 = hash_set<string>();
map<string, hash_set<string> > CChineseWordStatistic::s_mapWordCount2 = map<string, hash_set<string> >();
map<string, hash_set<string> > CChineseWordStatistic::s_mapWordCount3 = map<string, hash_set<string> >();
hash_map<string, ofstream*> CChineseWordStatistic::s_hmapOfstream1 = hash_map<string, ofstream*>();
hash_map<string, ofstream*> CChineseWordStatistic::s_hmapOfstream2 = hash_map<string, ofstream*>();
hash_map<string, ofstream*> CChineseWordStatistic::s_hmapOfstream3 = hash_map<string, ofstream*>();

CChineseWordStatistic::CChineseWordStatistic()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	m_wordNumWithSign = 0;
	m_wordNumWithoutSign = 0;
}

CChineseWordStatistic::~CChineseWordStatistic()
{

}

FTW_RESULT CChineseWordStatistic::ChineseWordStatistic( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring wstr = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	if( _wcsicmp( szFileName + nLen - wstr.length(), wstr.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CChineseWordStatistic* pInst = CChineseWordStatistic::GetInst();
	if ( pInst->BeLangDif(strFileName))
	{
		//pInst->DoStatistic(strFileName);
		pInst->StaticsLanaguage(strFileName);
	}
	return _FTW_CONTINUNE;
}

bool CChineseWordStatistic::BeLangDif( string& strFileName )
{
	if(m_pSplitInfo->BeLangDif(strFileName))
		return true;
	else
		return false;
}

void CChineseWordStatistic::DoStatistic( string& strFileName )
{
	int32 nCount = 0;
	int nPos = strFileName.rfind("/");
	string strRealFileName = strFileName;
	if (nPos != -1)
	{
		strRealFileName = strFileName.substr(nPos + 1, strFileName.length() - nPos - 1);
	}
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	replace_all_distinct(strRealFileName, strLuaCfgExtend, ".txt", 0);
	strRealFileName = "F:/WordCount/" + strRealFileName;
	ofstream ofsWordCount(strRealFileName.c_str());

	int nPos1 = -1;
	nPos1 = strFileName.find("langsrc/");
	nPos1 += strlen("langsrc/");
	int nPos2 = -1;
	if (nPos1 != -1)
	{
		nPos2 = strFileName.find("/", nPos1 + 1);
	}
	string strDirName = strFileName.substr(nPos1, nPos2 - nPos1);
	s_mapWordCount2.insert(make_pair(strDirName, hash_set<string>()));
	s_mapWordCount3.insert(make_pair(strDirName, hash_set<string>()));
	
	string strTotalWordTxtFileName1 = "F:/WordCount1/" + strDirName + "1.txt";
	string strTotalWordTxtFileName2 = "F:/WordCount1/" + strDirName + "2.txt";
	string strTotalWordTxtFileName3 = "F:/WordCount1/" + strDirName + "3.txt";
	
	if (s_hmapOfstream1.find(strDirName) == s_hmapOfstream1.end())
	{
		s_hmapOfstream1.insert(make_pair(strDirName, new ofstream(strTotalWordTxtFileName1.c_str())));
	}
	if (s_hmapOfstream2.find(strDirName) == s_hmapOfstream2.end())
	{
		s_hmapOfstream2.insert(make_pair(strDirName, new ofstream(strTotalWordTxtFileName2.c_str())));
	}
	if (s_hmapOfstream3.find(strDirName) == s_hmapOfstream3.end())
	{
		s_hmapOfstream3.insert(make_pair(strDirName, new ofstream(strTotalWordTxtFileName3.c_str())));
	}

	COriFileOperate oriFile(strFileName);
	//map<EFieldType,VecElementCol> mapSourceFileData;
	//ReadFileToMap(strFileName, mapSourceFileData);
	//VecElementCol& vecElementCol = mapSourceFileData[eFT_BotBegin];
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strContent;
	for (int nRow = 0; nRow < nRowNum; nRow++)
	{
		strContent = oriFile.GetDataByRowCol(nRow,0);
		for (int nCol = 1; nCol < nColNum; nCol++)
		{
			if (!oriFile.BeNumber(nRow,nCol))
			{
				strContent = oriFile.GetDataByRowCol(nRow,nCol);
				string strTemp = "";

				bool nFlag = false;
				for (int i = 0; i < (int)strContent.length(); i++)
				{
					if (strContent[i] > 128 || strContent[i] < 0)
					{
						nFlag = true;
						//break;
						s_nCount++;
						i++;
					}
				}
				if (nFlag)
				{
					string strContentCopy = strContent.substr(1, strContent.length() - 2);
					*(s_hmapOfstream1[strDirName]) << strContentCopy ;
					s_ofsWordCount1 << strContentCopy;

					if (strContentCopy.length() >= 12)
					{
						s_mapWordCount2[strDirName].insert(strContentCopy);
						s_hsetWordCount2.insert(strContentCopy);
					}
					else
					{
						*(s_hmapOfstream2[strDirName]) << strContentCopy;
						s_ofsWordCount2 << strContentCopy;
					}

					s_mapWordCount3[strDirName].insert(strContentCopy);
					s_hsetWordCount3.insert(strContentCopy);
				}
			}
		}
	}
}

void CChineseWordStatistic::PrintWordStatisticResult()
{
	printf("汉字总数：%d\n", s_nCount);
}

void CChineseWordStatistic::WriteAllWordToTxt()
{
	typedef hash_set<string>::iterator IterHashSet;
	for (IterHashSet iter = s_hsetWordCount2.begin(); iter != s_hsetWordCount2.end(); ++iter)
	{
		s_ofsWordCount2 << *iter << "\n";
	}

	for (IterHashSet iter = s_hsetWordCount3.begin(); iter != s_hsetWordCount3.end(); ++iter)
	{
		s_ofsWordCount3 << *iter << "\n";
	}

	typedef hash_map<string, ofstream*>::iterator IterHmapOfs;
	typedef hash_set<string>::iterator IterHsetStr;
	for (IterHmapOfs iterOfs = s_hmapOfstream2.begin(); iterOfs != s_hmapOfstream2.end(); ++iterOfs)
	{
		string strDirName = iterOfs->first;
		for (IterHsetStr iterStr = s_mapWordCount2[strDirName].begin(); iterStr != s_mapWordCount2[strDirName].end(); ++iterStr)
		{
			*(iterOfs->second) << *iterStr;
		}
	}
	
	for (IterHmapOfs iterOfs = s_hmapOfstream3.begin(); iterOfs != s_hmapOfstream3.end(); ++iterOfs)
	{
		string strDirName = iterOfs->first;
		for (IterHsetStr iterStr = s_mapWordCount3[strDirName].begin(); iterStr != s_mapWordCount3[strDirName].end(); ++iterStr)
		{
			*(iterOfs->second) << *iterStr;
		}
	}
}

void CChineseWordStatistic::StaticsLanaguage(string strFileName)
{
	COriFileOperate oriFile(strFileName);
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strContent;
	for (int nRow = 0; nRow < nRowNum; nRow++)
	{
		strContent = oriFile.GetDataByRowCol(nRow,0);
		for (int nCol = 1; nCol < nColNum; nCol++)
		{
			if (!oriFile.BeNumber(nRow,nCol))
			{
				strContent = oriFile.GetDataByRowCol(nRow,nCol);
				string strTemp = "";

				bool nFlag = false;
				for (int i = 0; i < (int)strContent.length(); i++)
				{
					bool bIgnore = false;
					if (strContent[i] > 128 || strContent[i] < 0)
					{
						string strThis = strContent.substr(i,2);
						if (strThis == "，" || strThis == "。")
						{
							bIgnore = true;
						}
						i++;
					}
					else
					{
						if (strContent[i] == ',' || strContent[i] == '.')
						{
							bIgnore = true;
						}
					}
					m_wordNumWithSign++;
					if (!bIgnore)
						m_wordNumWithoutSign++;
				}
			}
		}
	}
}

void CChineseWordStatistic::StaticsLanaguage(string strFileName,int& staticOne,int& staticTwo)
{
	staticOne = 0;
	staticTwo = 0;
	COriFileOperate oriFile(strFileName);
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strContent;
	for (int nRow = 0; nRow < nRowNum; nRow++)
	{
		strContent = oriFile.GetDataByRowCol(nRow,0);
		for (int nCol = 1; nCol < nColNum; nCol++)
		{
			if (!oriFile.BeNumber(nRow,nCol))
			{
				strContent = oriFile.GetDataByRowCol(nRow,nCol);
				string strTemp = "";

				bool nFlag = false;
				for (int i = 0; i < (int)strContent.length(); i++)
				{
					bool bIgnore = false;
					if (strContent[i] > 128 || strContent[i] < 0)
					{
						string strThis = strContent.substr(i,2);
						if (strThis == "，" || strThis == "。")
						{
							bIgnore = true;
						}
						i++;
					}
					else
					{
						if (strContent[i] == ',' || strContent[i] == '.')
						{
							bIgnore = true;
						}
					}
					staticOne++;
					if (!bIgnore)
						staticTwo++;
				}
			}
		}
	}
}

void CChineseWordStatistic::PrintTStaticResult()
{
	ofstream outFile("worldStatics.txt");
	outFile<<"带标点符号字数为:"<<m_wordNumWithSign<<endl;
	outFile<<"不带标点符号字数为:"<<m_wordNumWithoutSign<<endl;
	outFile.close();
}

void CChineseWordStatistic::StartProcess()
{
	string strPath = m_pSplitInfo->GetSrcPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	FileTreeWalk(wstrPath.c_str(), ChineseWordStatistic, NULL);
	FileTreeWalk(gbk_to_utf16(m_pSplitInfo->GetCfgTempPath()).c_str(), ChineseWordStatistic, NULL);
	PrintTStaticResult();
}

void CChineseWordStatistic::FolderCompare(string& strOne,string& strAnother)
{
	m_vecTemp.clear();
	wstring wstrOne = gbk_to_utf16(strOne);
	wstring wstrAnother = gbk_to_utf16(strAnother);
	FileTreeWalk(wstrOne.c_str(), RecordFilelist, NULL);
	m_vecCompareOne = m_vecTemp;
	m_vecTemp.clear();
	FileTreeWalk(wstrAnother.c_str(), RecordFilelist, NULL);
	m_vecCompareAnother = m_vecTemp;
	vector<pair<string,int> > vecResult;
	vector<string> vecCommon;
	vector<string> vecOneExist;
	vector<string> vecAnotherExist;
	for (int i=0;i<(int)m_vecCompareOne.size();++i)
	{
		string strFile = m_vecCompareOne[i];
		replace_all_distinct(strFile,strOne,strAnother,0);
		ifstream inFile(strFile.c_str());
		if (!inFile)
		{
			vecOneExist.push_back(m_vecCompareOne[i]);
		}
		else
		{
			string strTemp = strFile;
			replace_all_distinct(strTemp,strAnother,"",0);
			vecCommon.push_back(strTemp);
			vector<string>::iterator iter;
			iter = find(m_vecCompareAnother.begin(),m_vecCompareAnother.end(),strFile);
			m_vecCompareAnother.erase(iter);
		}
		inFile.close();
	}
	int resultOne = 0;
	int resultTwo = 0;
	int resultThree = 0;
	vecAnotherExist = m_vecCompareAnother;
	ofstream out("compareResult.txt");
	out<<strOne<<"路径存在，而"<<strAnother<<"不存在文件统计信息"<<endl;
	for (int i=0;i<(int)vecOneExist.size();++i)
	{
		int staticOne,staticTwo;
		StaticsLanaguage(vecOneExist[i],staticOne,staticTwo);
		out<<vecOneExist[i]<<"带标点符号字数为="<<staticOne<<" 不带标点符号字数为="<<staticTwo<<endl;
		resultOne += staticOne;
	}
	out<<"文件路径未变化，前后文字变化统计信息"<<endl;
	for (int i=0;i<(int)vecCommon.size();++i)
	{
		string strFile = vecCommon[i];
		string strOld = strOne + strFile;
		string strNew = strAnother + strFile;
		int oldStaticOne,oldStaticAno;
		int newStaticOne,newStaticAno;
		StaticsLanaguage(strOld,oldStaticOne,oldStaticAno);
		StaticsLanaguage(strNew,newStaticOne,newStaticAno);
		out<<strFile<<"\t上次发送字数信息："<<"带标点符号字数为="<<oldStaticOne<<" 不带标点符号字数为="<<oldStaticAno<<endl
			<<"\t新版本字数信息："<<"带标点符号字数为="<<newStaticOne<<" 不带标点符号字数为="<<newStaticAno<<endl
			<<"\t相差字数="<<newStaticOne-oldStaticOne<<endl;
		resultTwo +=newStaticOne-oldStaticOne;
	}
	out<<"新增加文件文字统计信息"<<endl;
	for (int i=0;i<(int)vecAnotherExist.size();++i)
	{
		int staticOne,staticTwo;
		StaticsLanaguage(vecAnotherExist[i],staticOne,staticTwo);
		out<<vecAnotherExist[i]<<"带标点符号字数为="<<staticOne<<" 不带标点符号字数为="<<staticTwo<<endl;
		resultThree += staticOne;
	}
	out<<endl<<endl;
	out<<"新版本下不存在的文件，但发给老外时存在，文字共："<<resultOne<<endl;
	out<<"新老版本相同文件，总字数增加＝"<<resultTwo<<endl;
	out<<"新增加的文件，总字数＝"<<resultThree;
	out.close();
}

void CChineseWordStatistic::RecordFile(string& strFile)
{
	m_vecTemp.push_back(strFile);
}

FTW_RESULT CChineseWordStatistic::RecordFilelist( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring wstr = L".dif";
	if( _wcsicmp( szFileName + nLen - wstr.length(), wstr.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CChineseWordStatistic* pInst = CChineseWordStatistic::GetInst();
	pInst->RecordFile(strFileName);
	return _FTW_CONTINUNE;
}
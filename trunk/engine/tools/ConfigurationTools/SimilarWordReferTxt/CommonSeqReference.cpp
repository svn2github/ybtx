#include "stdafx.h"
#include <cmath>
#include "CommonSeqReference.h"
#include "TRandomMersenne.h"
#include "LongestCommonSequence.h"
#include "COriFileOperate.h"
#include "common.h"
#include "CExpLog.h"

CCommonSeqReference::CCommonSeqReference()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	ReadReferneceTxt();
}

CCommonSeqReference::~CCommonSeqReference()
{
	
}

FTW_RESULT CCommonSeqReference::WriteItemDescToVector( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	CCommonSeqReference* pInst = CCommonSeqReference::GetInst();
	if (pInst->BeInResolveList(strFileName))
	{
		pInst->DoWriteItemDesc(strFileName);
	}
	return _FTW_CONTINUNE;
}

FTW_RESULT CCommonSeqReference::WriteQuestDescToVector( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	CCommonSeqReference* pInst = CCommonSeqReference::GetInst();
	if (pInst->BeInResolveList(strFileName))
	{
		pInst->DoWriteQuestDesc(strFileName);
	}
	return _FTW_CONTINUNE;
}

bool CCommonSeqReference::BeInResolveList( const string& strFileName )
{
	if(m_pSplitInfo->BeInSplitList(strFileName))
		return true;
	else
		return false;
}

void CCommonSeqReference::DoWriteItemDesc( const string& strFileName )
{
	uint32 uPos = strFileName.rfind("/");
	string strRealName = strFileName.substr(uPos+1, strFileName.length()-uPos-1);
	string strLanFileName = m_pSplitInfo->GetLangName(strRealName);
	string strTemp = strFileName;
	COriFileOperate oriFile(strTemp);
	oriFile.CleanBlanRow();
	oriFile.CleanNoteRow();
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strColName;
	int nDescCol = -1;
	int nColNameRow = 0;
	for (int nCol = 0; nCol < nColNum; nCol++)
	{
		if (!oriFile.BeNumber(nColNameRow,nCol))
		{
			strColName = oriFile.GetDataByRowCol(nColNameRow,nCol);
			if (strColName == "Description")
			{
				nDescCol = nCol;
				break;
			}
		}
	}
	if (nDescCol == -1)
	{
		return;
	}
	string strItemName;
	string strItemDesc;
	string strItemNameH64;
	for (int nRow = nColNameRow + 1; nRow < nRowNum; nRow++)
	{
		if (!oriFile.BeNumber(nRow,0))
		{
			strItemName = oriFile.GetDataByRowCol(nRow,0);
		}
		strItemNameH64 = mbcs_to_MemH64(strItemName);

		if (!oriFile.BeNumber(nRow,nDescCol))
		{
			strItemDesc = oriFile.GetDataByRowCol(nRow,nDescCol);
			ReplaceWildcard(strItemDesc);
		}
		if (strItemDesc != "")
		{
			if (find(s_vecDesc.begin(), s_vecDesc.end(), strItemDesc) == s_vecDesc.end())
			{
				s_vecDesc.push_back(strItemDesc);
			}
		}
		s_hmapDesc2SrcIndex.insert(make_pair(strItemDesc, strItemNameH64));
		s_hmapSrcIndex2FileName.insert(make_pair(strItemNameH64, strLanFileName));
	}
}

void CCommonSeqReference::DoWriteQuestDesc( const string& strFileName )
{
	uint32 uPos = strFileName.rfind("/");
	string strRealName = strFileName.substr(uPos+1, strFileName.length()-uPos-1);
	string strLanFileName = m_pSplitInfo->GetLangName(strRealName);
	string strTemp = strFileName;
	COriFileOperate oriFile(strTemp);
	oriFile.CleanNoteRow();
	oriFile.CleanBlanRow();
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strQuestField;
	string strQuestContent;
	string strQuestName;
	string strQuestIndexH64;
	for (int nRow = 1; nRow < nRowNum; nRow++)
	{
		if (!oriFile.BeNumber(nRow,0))
		{
			strQuestName = oriFile.GetDataByRowCol(nRow,0);
		}

		if (!oriFile.BeNumber(nRow,1))
		{
			strQuestField = oriFile.GetDataByRowCol(nRow,1);
		}
		if (strQuestField == "任务接取对话" || strQuestField == "任务描述")
		{
			if (!oriFile.BeNumber(nRow,3))
			{
				strQuestContent = oriFile.GetDataByRowCol(nRow,3);
			}
			if (strQuestContent != "")
			{
				if (find(s_vecDesc.begin(), s_vecDesc.end(), strQuestContent) == s_vecDesc.end())
				{
					s_vecDesc.push_back(strQuestContent);
				}
				strQuestIndexH64 = mbcs_to_MemH64(strQuestName) + "_" ;
				if (strQuestField == "任务描述")
				{
					strQuestIndexH64 += "Description";
				}
				else if (strQuestField == "任务接取对话")
				{
					strQuestIndexH64 += "StartDialog";
				}
				s_hmapDesc2SrcIndex.insert(make_pair(strQuestContent, strQuestIndexH64));
				s_hmapSrcIndex2FileName.insert(make_pair(strQuestIndexH64, strLanFileName));
			}
		}
	}
}

void CCommonSeqReference::ProduceDescCommSeq()
{
	typedef vector<string>::iterator IterVecStr;
	IterVecStr iterNext;
	IterVecStr iter = s_vecDesc.begin();
	string strCurDesc;
	string strNextDesc;
	string strPreCommSeq;
	string strCurCommSeq;
	string strNextCommSeq;
	if (iter != s_vecDesc.end())
	{
		s_hmapDesc2CommSeqPos.insert(make_pair(*iter, s_vecDescCommSeq.size()));
		strPreCommSeq = *iter;
		iter++;
	}
	else
	{
		return;
	}
	for (; iter != s_vecDesc.end()-1; ++iter)
	{
		strCurDesc = *iter;
		strNextDesc = *(iter + 1);
		CLongestCommonSequence pLCSCur = CLongestCommonSequence(gbk_to_utf16(strPreCommSeq).c_str(), gbk_to_utf16(strCurDesc).c_str());
		pLCSCur.CommSeq();
		wstring wstrCommSeq = pLCSCur.GetCommSeq();
		strCurCommSeq = utf16_to_gbk(wstrCommSeq);
		CLongestCommonSequence pLCSNext = CLongestCommonSequence(gbk_to_utf16(strCurDesc).c_str(), gbk_to_utf16(strNextDesc).c_str());
		pLCSNext.CommSeq();
		wstrCommSeq = pLCSNext.GetCommSeq();
		strNextCommSeq = utf16_to_gbk(wstrCommSeq);
		if (strCurCommSeq.length() <= strPreCommSeq.length()/2) //与上一行求子串之后变化很大
		{
			s_vecDescCommSeq.push_back(strPreCommSeq);
			strPreCommSeq = strCurDesc;
		}
		else if (strCurCommSeq.length() <= strNextCommSeq.length()/2) //与下一行求子串后变化很大
		{
			s_vecDescCommSeq.push_back(strPreCommSeq);
			strPreCommSeq = strCurDesc;
		}
		else
		{
			strPreCommSeq = strCurCommSeq;
		}
		s_hmapDesc2CommSeqPos.insert(make_pair(strCurDesc, s_vecDescCommSeq.size()));
	}

	strCurDesc = *iter;
	CLongestCommonSequence pLCS = CLongestCommonSequence(gbk_to_utf16(strPreCommSeq).c_str(), gbk_to_utf16(strCurDesc).c_str());
	pLCS.CommSeq();
	wstring wstrCommSeq = pLCS.GetCommSeq();
	strCurCommSeq = utf16_to_gbk(wstrCommSeq);
	if (strCurCommSeq.length() <= strPreCommSeq.length()/2)
	{
		s_vecDescCommSeq.push_back(strPreCommSeq);
	}
	s_hmapDesc2CommSeqPos.insert(make_pair(strCurDesc, s_vecDescCommSeq.size()));
	s_vecDescCommSeq.push_back(strCurCommSeq);
}

uint32 CCommonSeqReference::GetCommDescIndex( const string& strCommDesc )
{
	int nLen = strCommDesc.length();
	TRandomMersenne* pRandom = new TRandomMersenne(7);
	uint32 nRandomNum = pRandom->BRandom();
	uint32  nSum = strCommDesc[0] ^ nRandomNum;
	for (int i=1; i<nLen; i++)
	{
		nRandomNum = pRandom->BRandom();
		nSum = nSum ^ strCommDesc[i] ^ nRandomNum;
	}
	int nStart = 0;
	while (s_hmapCommSeq.find(nSum) != s_hmapCommSeq.end())
	{
		nRandomNum = pRandom->BRandom();
		nSum = nSum ^ strCommDesc[nStart++] ^ nRandomNum;
		if (nStart == nLen)
		{
			nStart = 0;
		}
	}
	delete pRandom;
	return nSum;
}

void CCommonSeqReference::ProduceRefrence()
{
	typedef hash_map<string, uint32>::iterator IterHmapSU;
	typedef hash_multimap<string, string>::iterator IterHmmapSS;
	typedef hash_map<uint32, string>::iterator IterHmapUS;
	string strDesc;
	string strSrcIndex;
	string strCommSeq;
	int nCommSeqIndex;
	for (IterHmapSU iter=s_hmapDesc2CommSeqPos.begin(); iter!=s_hmapDesc2CommSeqPos.end(); ++iter)
	{
		strDesc = iter->first;
		strCommSeq = s_vecDescCommSeq[iter->second];
		bool nFind = false;
		for (IterHmapUS iterUs=s_hmapCommSeq.begin(); iterUs!=s_hmapCommSeq.end(); ++iterUs)
		{
			if (iterUs->second == strCommSeq)
			{
				nFind = true;
				nCommSeqIndex = iterUs->first;
				break;
			}
		}
		if (!nFind)
		{
			nCommSeqIndex = GetCommDescIndex(strCommSeq); 
			s_hmapCommSeq.insert(make_pair(nCommSeqIndex, strCommSeq));
		}
		pair<IterHmmapSS, IterHmmapSS> b = s_hmapDesc2SrcIndex.equal_range(strDesc);
		for (IterHmmapSS iterH=b.first; iterH!=b.second; ++iterH)
		{
			strSrcIndex = iterH->second;
			s_hmapSrcIndex2CommSeqIndex.insert(make_pair(strSrcIndex, nCommSeqIndex));
		}
	}
}

void CCommonSeqReference::OutputDescCommSeq( const char* szFileName )
{
	const char* szFileRealName = strrchr(szFileName, '/');
	szFileRealName++;
	m_strCommonFileName = szFileRealName;
	char charEnter[4] = {0x0D,0x00,0x0A,0x00};
	char charTab[2] = {0x09,0x00};
	CreateFileDirectory(szFileName);
	ofstream ofsCommSeq(szFileName, ios_base::binary | ios_base::out);

	char headArray[2];
	headArray[0] = uint8(15*16+15);
	headArray[1] = uint8(15*16+14);
	ofsCommSeq.write(headArray, 2);

	vector<wstring> vecColName;
	vecColName.push_back(L"Index");
	vecColName.push_back(L"CommonSequence");

	ofsCommSeq.write((const char*)vecColName[0].c_str(), vecColName[0].length()*2);
	ofsCommSeq.write(charTab, 2);
	ofsCommSeq.write((const char*)vecColName[1].c_str(), vecColName[1].length()*2);
	ofsCommSeq.write(charEnter, 4);

	typedef hash_map<uint32, string>::iterator IterHmapUS;
	uint32 uCommSeqIndex;
	wchar_t szCommonIndex[20];
	wstring wstrCommSeq;
	for (IterHmapUS iter=s_hmapCommSeq.begin(); iter!=s_hmapCommSeq.end(); ++iter)
	{
		uCommSeqIndex = iter->first;
		_ui64tow(uCommSeqIndex, szCommonIndex, 10);
		ofsCommSeq.write((char*)szCommonIndex, wcslen(szCommonIndex)*2);
		ofsCommSeq.write(charTab, 2);
		wstrCommSeq = gbk_to_utf16(iter->second);
		ofsCommSeq.write((const char*)wstrCommSeq.c_str(), wstrCommSeq.length()*2);
		ofsCommSeq.write(charEnter, 4);
	}
	ofsCommSeq.close();
}

void CCommonSeqReference::OutputRefrence( const char* szFileName )
{
	char charEnter[4] = {0x0D,0x00,0x0A,0x00};
	char charTab[2] = {0x09,0x00};

	vector<wstring> vecColName;
	vecColName.push_back(L"Project 1");
	vecColName.push_back(L"Key 1");
	vecColName.push_back(L"Project 2");
	vecColName.push_back(L"Key 2");
	vecColName.push_back(L"Mode of Referency");
	vecColName.push_back(L"Reference Description");
	CreateFileDirectory(szFileName);
	ofstream ofsRefrence(szFileName, ios_base::binary | ios_base::out);
	char headArray[2];
	headArray[0] = uint8(15*16+15);
	headArray[1] = uint8(15*16+14);
	ofsRefrence.write(headArray, 2);
	for (int i=0; i<(int)(vecColName.size()-1); i++)
	{
		ofsRefrence.write((const char*)vecColName[i].c_str(), vecColName[i].length()*2);
		ofsRefrence.write(charTab, 2);
	}
	ofsRefrence.write((const char*)vecColName[vecColName.size()-1].c_str(), vecColName[vecColName.size()-1].length()*2);
	ofsRefrence.write(charEnter, 4);

	typedef hash_map<string, uint32>::iterator IterHmapSU;
	uint32 uCommSeqIndex;
	char szCommonIndex[2048];
	string strSrcIndex;
	string strSrcProject;
	string strSrcKey;
	string strCommonProject;
	string strCommonKey;
	wchar_t* szMode = L"two-way";
	//wchar_t* szRefDesc = L"690108";
	wchar_t* szRefDesc = L"similar_content";
	for (IterHmapSU iter=s_hmapSrcIndex2CommSeqIndex.begin(); iter!=s_hmapSrcIndex2CommSeqIndex.end(); ++iter)
	{
		strSrcIndex = iter->first;
		uCommSeqIndex = iter->second;
		_i64toa(uCommSeqIndex, szCommonIndex, 10);
		strSrcProject = GetProject(s_hmapSrcIndex2FileName[strSrcIndex]);
		strSrcKey = GetKeyPrefix(s_hmapSrcIndex2FileName[strSrcIndex]) + strSrcIndex;
		strCommonProject = GetProject(m_strCommonFileName);
		strCommonKey = GetKeyPrefix(m_strCommonFileName) + szCommonIndex;
		ofsRefrence.write((const char*)utf8_to_utf16(strSrcProject).c_str(), utf8_to_utf16(strSrcProject).length()*2);
		ofsRefrence.write(charTab, 2);
		ofsRefrence.write((const char*)utf8_to_utf16(strSrcKey).c_str(), utf8_to_utf16(strSrcKey).length()*2);
		ofsRefrence.write(charTab, 2);
		ofsRefrence.write((const char*)utf8_to_utf16(strCommonProject).c_str(), utf8_to_utf16(strCommonProject).length()*2);
		ofsRefrence.write(charTab, 2);
		ofsRefrence.write((const char*)utf8_to_utf16(strCommonKey).c_str(), utf8_to_utf16(strCommonKey).length()*2);
		ofsRefrence.write(charTab, 2);
		ofsRefrence.write((char*)szMode, wcslen(szMode)*2);
		ofsRefrence.write(charTab, 2);
		ofsRefrence.write((char*)szRefDesc, wcslen(szRefDesc)*2);
		ofsRefrence.write(charEnter, 4);
	}
	ofsRefrence.close();
}

string CCommonSeqReference::GetProject( string& strFileName )
{
	if (!m_mapReference.count(strFileName))
	{
		ofstream logFile("NoReferInfo.txt",ios_base::out | ios_base::app);
		logFile<<strFileName<<"无Projekt"<<endl;
		logFile.close();

		if(GetLogToFile())
		{
			stringstream strExp;
			strExp << strFileName << "无Projekt"; 
			PrintLog(strExp.str());
		}

		return "没有Projekt信息";
	}
	return m_mapReference[strFileName].m_strProjekt;
}

string CCommonSeqReference::GetKeyPrefix( string& strFileName )
{
	if (!m_mapReference.count(strFileName))
	{
		ofstream logFile("NoReferInfo.txt",ios_base::out | ios_base::app);
		logFile<<strFileName<<"无Keyprefix"<<endl;
		logFile.close();

		if(GetLogToFile())
		{
			stringstream strExp;
			strExp << strFileName << "无Keyprefix"; 
			PrintLog(strExp.str());
		}

		return "没有Keyprefix信息";
	}
	return m_mapReference[strFileName].m_strPreFix;
}

void CCommonSeqReference::ReadReferneceTxt()
{
	if (!m_mapReference.empty())
		return;
	//string strName("Warrior King - Projectstructur.txt");
	const string& strName = CSplitInfo::GetInst()->GetProjectStructerFileName();
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strName,vecTemp);
	int colNum = vecTemp[0].size();
	vector<int> vecSIndex;
	vecSIndex.resize(3);
	vecSIndex[0] = find(vecTemp[0].begin(),vecTemp[0].end(),"File") - vecTemp[0].begin();
	vecSIndex[1] = find(vecTemp[0].begin(),vecTemp[0].end(),"Project") - vecTemp[0].begin();
	vecSIndex[2] = find(vecTemp[0].begin(),vecTemp[0].end(),"Keypr?fix") - vecTemp[0].begin();
	for (int i =1;i<(int)vecTemp.size();++i)
	{
		string strFileName = vecTemp[i][vecSIndex[0]] + ".txt";
		string strProjekt = vecTemp[i][vecSIndex[1]];
		string strKeyPrefix = vecTemp[i][vecSIndex[2]];
		ReferenceInfor referData;
		referData.m_strProjekt = strProjekt;
		referData.m_strPreFix = strKeyPrefix;
		m_mapReference[strFileName] = referData;
	}
}

void CCommonSeqReference::ReadUtf8Txt(const string& strFileName,vector<vector<string> >& vecRef )
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	string str;
	while(getline(inFile,str))
	{
		vecRef.push_back(vector<string>());
		int pos = -1;
		while ((pos = str.find('\t')) != -1)
		{
			string strTemp = str.substr(0,pos);
			vecRef[vecRef.size()-1].push_back(strTemp);
			str = str.substr(pos+1,str.length()-pos-1);
		}
		vecRef[vecRef.size()-1].push_back(str);
	}
	inFile.close();
}

void CCommonSeqReference::ReplaceWildcard(string& str)
{
	int percentPos = 0;
	int argNum = 1;
	while ((percentPos = str.find('%',percentPos)) != -1)
	{
		if (percentPos == str.length()-1)
			break;
		if (str[percentPos+1] == '%')
		{
			percentPos += 2;
			continue;
		}
		if (percentPos>(int)str.length()-1)
			break;
		int nextPercentPos = str.find('%',percentPos+1);
		int wellPos = str.find('#',percentPos+1);
		int dPos = str.find('d',percentPos+1);
		int fPos = str.find('f',percentPos+1);
		int sPos = str.find('s',percentPos + 1);
		int nearPos = -1;
		if (dPos == -1 && fPos ==-1 && sPos == -1)
			break;
		if (dPos != -1 && fPos != -1)
		{
			nearPos = dPos < fPos ? dPos : fPos;
			if (sPos != -1)
			{
				nearPos = sPos < nearPos ? sPos : nearPos;
			}
		}
		else if(dPos != -1)
		{
			nearPos = dPos;
			if (sPos != -1)
			{
				nearPos = sPos < nearPos ? sPos : nearPos;
			}
		}
		else if (fPos != -1)
		{
			nearPos = fPos;
			if (sPos != -1)
			{
				nearPos = sPos < nearPos ? sPos : nearPos;
			}
		}
		else
		{
			nearPos = sPos;
		}
		if (nearPos == -1)
		{
			break;
		}
		bool bContinue = false;
		for (int i = percentPos+1; i< nearPos; ++i)
		{
			if (str[i] != '.' && !(str[i] >= '0' && str[i] <= '9'))
			{
				bContinue = true;
			}
		}
		if (bContinue)
		{
			++percentPos;
			continue;
		}

		string strWildcard;
		char numArray[10];
		numArray[9] = '\0';
		_itoa(argNum,numArray,10);
		string strHead = "#arg";
		strHead = strHead + numArray;
		strHead = strHead + "(";
		strWildcard = str.substr(percentPos,nearPos-percentPos+1);
		strWildcard = strHead + strWildcard;
		strWildcard = strWildcard + ")";
		str = str.replace(percentPos,nearPos-percentPos+1,strWildcard);
		++argNum;
		percentPos = percentPos + strWildcard.length();
	}
}

void CCommonSeqReference::BeginItemDesc()
{
	CleanMemData();
	string strPath = CSplitInfo::GetInst()->GetTransExportPath();
	string strDescName = strPath + "/" + "ItemDescCommonText.txt";
	string strReferName = strPath + "/" + "ItemCommDescTwoRefer.txt";
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetItemPath()).c_str(), WriteItemDescToVector, NULL );
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetItemTempPath()).c_str(), WriteItemDescToVector, NULL );
	ProduceDescCommSeq();
	ProduceRefrence();
	OutputDescCommSeq(strDescName.c_str());
	OutputRefrence(strReferName.c_str());
}

void CCommonSeqReference::BeginQuestDesc()
{
	CleanMemData();
	string strPath = CSplitInfo::GetInst()->GetTransExportPath();
	string strDescName = strPath + "/" + "QuestDescCommonText.txt";
	string strReferName = strPath + "/" + "QuestCommDescTwoRefer.txt";
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetQuestPath()).c_str(), WriteQuestDescToVector, NULL );
	FileTreeWalk( gbk_to_utf16(CSplitInfo::GetInst()->GetQuestTempPath()).c_str(), WriteQuestDescToVector, NULL );
	CCommonSeqReference::GetInst()->ProduceDescCommSeq();
	CCommonSeqReference::GetInst()->ProduceRefrence();
	CCommonSeqReference::GetInst()->OutputDescCommSeq(strDescName.c_str());
	CCommonSeqReference::GetInst()->OutputRefrence(strReferName.c_str());
}

void CCommonSeqReference::CleanMemData()
{
	s_hmapDesc2SrcIndex.clear();
	s_hmapSrcIndex2FileName.clear();
	s_hmapDesc2CommSeqPos.clear();
	s_hmapSrcIndex2CommSeqIndex.clear();
	s_vecDesc.clear();
	s_vecDescCommSeq.clear();
	s_hmapCommSeq.clear();
	m_strCommonFileName.clear();
}
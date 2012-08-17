#include "stdafx.h"
#include "CAppSrc2Txt.h"
#include "CSplitInfo.h"
#include "common.h"
#include "DifInfo.h"
#include "COriFileOperate.h"
#include "CRecordManager.h"
#include "ConsoleInfo.h"
#include "CXmlConfig.inl"
#include <sstream>
#include "tinyxml/tinyxml.h"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

using namespace std;
#define CHUNKNAME "MakeAppFinallyTxt"

CDif2Txt::CDif2Txt()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	m_strTgt = m_pSplitInfo->GetTgtPath();
	m_strSrc = m_pSplitInfo->GetSrcPath();
	m_iRecord=0;
	CRecordManager::SetChunkName(CHUNKNAME);
}

CDif2Txt::~CDif2Txt()
{

}


void CDif2Txt::StartProcess()
{
	wstring wstrPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	string strPostfix = utf16_to_gbk(wstrPostfix);
	strPostfix.erase(strPostfix.begin());
	strPostfix = strPostfix + "转txt";
	CConsoleInfo::GetInst()->PrintFunction(strPostfix);
	string strPath = m_pSplitInfo->GetSrcPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	FileTreeWalk(wstrPath.c_str(), Dif2Txt, NULL);
	string strCfgTempPath = m_pSplitInfo->GetCfgTempPath();
	wstring wstrCfgTempPath = gbk_to_utf16(strCfgTempPath);
	FileTreeWalk(wstrCfgTempPath.c_str(),Dif2Txt,NULL);
}

FTW_RESULT CDif2Txt::Dif2Txt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	wstring strNdfPostfix = CSplitInfo::GetInst()->WGetCppCfgExtend();
    string strLangSrc = "/"+CSplitInfo::GetInst()->GetLangSrcFolder()+"/";

	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) 
		&& _wcsicmp( szFileName + nLen - strNdfPostfix.length(),strNdfPostfix.c_str() ) )
		return _FTW_CONTINUNE;

	CDif2Txt* pManager = CDif2Txt::GetInst();
	string strFileName = utf16_to_gbk(szFileName);

	if (pManager->BeRequireToTxt(strFileName) && CRecordManager::GetInst()->BeChanged(strFileName))
	{
		if(strFileName.find(strLangSrc)==string::npos)
		{
		     pManager->Change2Txt(strFileName);
		}
	}
	return _FTW_CONTINUNE;
}

void CDif2Txt::Change2Txt(string& strFileName)
{
	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	CheckMultiTable(strFileName);
	string strCfgTempPath = m_pSplitInfo->GetCfgTempPath();
	string strDstName = strFileName;
	if(strDstName.find(strCfgTempPath.c_str())==string::npos)
	{
		replace_all_distinct(strDstName,m_strSrc,m_strTgt,0);
	}
	else 
	{
		replace_all_distinct(strDstName,strCfgTempPath,m_strTgt,0);
	}
	size_t uSize = strDstName.length();
	strDstName.replace(uSize-3, 3, "txt");
	SQR_TRY
	ofstream outFile(strDstName.c_str());
	if (!outFile)
	{
		string strMsg = strDstName + "在打开时发生错误，请检查文件是否可写";
		if(GetLogToFile())
		{
			PrintLog(strMsg);
		}
		else
		{
			MessageBox(0,strMsg.c_str(),NULL,0);
		}
		exit(1);
	}
	COriFileOperate oriFlie(strFileName);
	oriFlie.CleanNoteRow();
	if(!m_pSplitInfo->GetBeKeepBlankLine(strOldName))
		oriFlie.CleanBlanRow();
	int startRow = 0;
	bool cread = false;
	size_t allRowNum = oriFlie.GetRowCount();
	size_t allColNum = 0;
	string strOriFileName=strFileName;
	if(strFileName.find(strCfgTempPath.c_str())!=string::npos)
	{
		replace_all_distinct(strOriFileName,strCfgTempPath,m_strSrc,0);
	}
	CDifInfo difInfo(strOriFileName.c_str());
	if (difInfo.BeMultiRowTable())
	{
		startRow = 1;
	}
	if(m_pSplitInfo->GetCReadTable(strOldName))
	{
		cread = true;
	}
	allColNum = oriFlie.GetColCountByRow(0);
	for (int i=startRow;i<allRowNum;++i)
	{
		for (int j=0;j<allColNum;++j)
		{
			outFile<<oriFlie.GetDataByRowCol(i,j);
			if (j<allColNum-1)
				outFile<<"\t";
		}
		if(!cread)
		{
			if(i>1)
			{
				if(!startRow)
				{
					if(FIRSTROW.find(oriFlie.GetDataByRowCol(i,0))!=FIRSTROW.end())
					{
						ostringstream str;
						str<<"单行配置表："<<strFileName<<"的第"<<i+1<<"行重复，重复的索引为："<<oriFlie.GetDataByRowCol(i,0);

						if(GetLogToFile())
						{
							PrintLog(str.str());
						}
						else
						{
							MessageBox(0,str.str().c_str(),"单行表索引重复",0);
						}

					}
		
				}
			}
		}
		FIRSTROW.insert(oriFlie.GetDataByRowCol(i,0));
		outFile<<endl;
	}
	FIRSTROW.clear();
	outFile.close();
	SQR_CATCH(exp)
	{
		string strMsg = "CDif2Txt生成txt文件:" + strDstName + "时发生错误,错误类型为:" + exp.ErrorTitle();
		if(GetLogToFile())
		{
			PrintLog(strMsg);
		}
		else
		{
			MessageBox(0,strMsg.c_str(),NULL,0);
		}
		exit(1);
	}
	SQR_TRY_END
	vector<string> vecChild;
	vecChild.push_back(strDstName);
	CRecordManager::GetInst()->AddFileRecord(strFileName);
	CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
	for (int i=0;i<vecChild.size();++i)
	{
		CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
	}
 	CConsoleInfo::GetInst()->EndToPrint();
}

bool CDif2Txt::BeRequireToTxt(string& strFileName)
{
	if (m_pSplitInfo->BeInSplitList(strFileName) || m_pSplitInfo->BeLangDif(strFileName))
		return false;
	else
		return true;
}
bool CDif2Txt::CheckMultiTable(string& strFileName)
{
	string strOldName = GetFileNameFromPath(strFileName);
	string strCfgTempSrc = m_pSplitInfo->GetCfgTempPath();
	string strOriFileName = strFileName;
	if(strFileName.find(strCfgTempSrc.c_str())!=string::npos)
	{
		replace_all_distinct(strOriFileName,strCfgTempSrc,m_strSrc,0);
	}
	CDifInfo difInfo(strOriFileName.c_str());
	if (!difInfo.BeMultiRowTable())
	{
		return false;
	}
	string strLuaExtend = m_pSplitInfo->GetLuaCfgExtend();
	replace_all_distinct(strOriFileName,strLuaExtend,".xml",0);
	ifstream config(strOriFileName.c_str());
	if(!config)
		return false;
	CXmlConfig *pXmlCfg = new CXmlConfig();
	pXmlCfg->LoadConfig( "config" , config );
	config.close();
	TiXmlNode* IndexKey = pXmlCfg->Get<TiXmlElement*>("IndexKey");
	MultiTableXmlTree(IndexKey);
	if(!m_mVectorInt.empty())
	{
		COriFileOperate oriFlie(strFileName);
		oriFlie.CleanNoteRow();
		if(!m_pSplitInfo->GetBeKeepBlankLine(strOldName))
			oriFlie.CleanBlanRow();
		int allRowNum = oriFlie.GetRowCount();
		for(int i=1;i<allRowNum;i++)
		{
			vector<string> vec;
			vector<string> compare;
			for(int j=0;j<=m_iMax;j++)
			{
				vec.push_back(oriFlie.GetDataByRowCol(i,j));
			}
			for(int j=1;j<=m_iMax;j++)
			{
				compare.push_back(oriFlie.GetDataByRowCol(i,j));
			}
			bool mark = false;
			map<vector<string>,int>::iterator iterm=m_mVectorInt.begin();
			if((iterm->first)[0]=="RandomKey")
				mark=true;
			map<vector<string>,int>::iterator m_iter = m_mVectorInt.begin();
			for(;m_iter!=m_mVectorInt.end();m_iter++)
			{
				if(compare==(m_iter->first))
					mark=true;
			}
			if(mark==true)
			{
				if(i>1)
				{
					map<vector<string>,int>::iterator iter = m_mMapVecBuf.begin();
					for(;iter!=m_mMapVecBuf.end();iter++)
					{
						if(vec==(iter->first))
						{
							ostringstream str;
							str<<"多行配置表："<<strFileName<<"的第"<<i+1<<"行和第"<<iter->second+1<<"行重复";
							MessageBox(0,str.str().c_str(),"重复",0);
						}

					}
					m_mMapVecBuf.insert(make_pair(vec,i));
				}
			}	
		}
		m_mMapVecBuf.clear();
		m_mVectorInt.clear();
	}

	return true;
}
void CDif2Txt::MultiTableXmlTree(TiXmlNode * node)
{
	for (TiXmlNode* Index = node->FirstChild("Index");Index;Index = Index->NextSibling("Index"))
	{
		if(Index!=NULL)
		{
			TiXmlElement* pSIndexElement = Index->ToElement();
			string strSIndex = pSIndexElement->Attribute("IndexKey");
			m_iRecord++;
			m_iMax = m_iRecord;
			m_sString.push(strSIndex);
			if(strSIndex!="DefaultKey")
			{
				if(Index->FirstChild("Col"))
				{
					m_sMax=m_sString;
					m_vString.clear();
					while(!m_sMax.empty())
					{
						string str =m_sMax.top();
						m_sMax.pop();
						m_vString.push_back(str);
					}
					vector<string>::reverse_iterator riter;
					riter=m_vString.rbegin();
					vector<string> temp;
					for(;riter!=m_vString.rend();riter++)
					{
						temp.push_back(*riter);
					}
					m_mVectorInt.insert(make_pair(temp,m_iMax));
					temp.clear();
				}
			}
			//递归遍历所有节点和叶子
			MultiTableXmlTree(Index);
			m_sString.pop();
			m_iRecord--;
		}
	}
}
#pragma warning(pop)

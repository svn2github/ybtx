#include "stdafx.h"
#include "DataType2Xml.h"
#include "CXmlConfig.inl"
#include "DifInfo.h"
#include "COriFileOperate.h"
#include "ConsoleInfo.h"
#include "shellapi.h"
#include "CRecordManager.h"
#include <sstream>
#include "CXmlConfig.h"
#include "CExpLog.h"

//#define CONVERT_XML
#define CHUNKNAME "MakeXMLOfTxt"

CDataType2Xml::CDataType2Xml()
{
	m_Convert = false;
	m_pSplitInfo = CSplitInfo::GetInst();
	m_strCfgTempPath = m_pSplitInfo->GetCfgTempPath();
	m_strTgtPath = m_pSplitInfo->GetTgtPath();
	m_strSrcPath = m_pSplitInfo->GetSrcPath();
	CRecordManager::SetChunkName(CHUNKNAME);
//	CXmlConfig XmlCfg;
//	if(!XmlCfg.LoadConfig("root", "MakeXmlOfTxt.xml"))
//		GenErr("MakeXmlOfTxt.xml不存在");
//	TiXmlNode* pNode = XmlCfg.GetRootNode();
///	string str = pNode->FirstChild("CONVERT_XML")->ToElement()->GetText();
//	if (str == "true")m_Convert = true;
}

CDataType2Xml::~CDataType2Xml()
{
	m_pSplitInfo = NULL;
}

void CDataType2Xml::StartProcess()
{
	CConsoleInfo::GetInst()->PrintFunction("生成配置表列信息XML");
	string strPath = m_pSplitInfo->GetSrcPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	wstring wstrCfgTempPath = gbk_to_utf16(m_strCfgTempPath);
	FileTreeWalk(wstrPath.c_str(), WriteToXml, NULL);
	FileTreeWalk(wstrCfgTempPath.c_str(), WriteToXml, NULL);
}

FTW_RESULT CDataType2Xml::WriteToXml(const wchar_t* wszFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen(wszFileName);
	if (!_wcsicmp(wszFileName + nLen - 4, L".svn") && eFlag == _FTW_DIR)
		return _FTW_IGNORE;
	if (eFlag != _FTW_FILE || nLen <= 4)
		return _FTW_CONTINUNE;	
	wstring wstrLua = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	wstring wstrCpp = CSplitInfo::GetInst()->WGetCppCfgExtend();
	if (_wcsicmp( wszFileName + nLen - wstrLua.length(), wstrLua.c_str() ) && _wcsicmp( wszFileName + nLen - wstrCpp.length(), wstrCpp.c_str() ))
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(wszFileName);
	if (!CRecordManager::GetInst()->BeChanged(strFileName))
		return _FTW_CONTINUNE;
	CDataType2Xml* pInst = CDataType2Xml::GetInst();
	pInst->CreateXml(strFileName);
	return _FTW_CONTINUNE;
}

void CDataType2Xml::CreateXml(string& strFileName)
{
	const string& strOrilang = CSplitInfo::GetInst()->GetOriLang();
	if(strFileName.find(strOrilang) != -1 ||m_pSplitInfo->BeInSplitList(strFileName))
		return;
	string strOldFileName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldFileName.c_str());
	string strXml = GetXmlPath(strFileName);
	string strTxt = strXml.substr(0,strXml.size()-4);
	strTxt.append(".txt");
	string strOriFileName=strFileName;
	string strCfgTempPath = m_pSplitInfo->GetCfgTempPath();
	if(strFileName.find(strCfgTempPath.c_str())!=string::npos)
	{
		replace_all_distinct(strOriFileName,strCfgTempPath,m_strSrcPath,0);
	}
	CDifInfo difInfo(strOriFileName.c_str());
	if (BeMultiRowTable(strOriFileName)&&(m_Convert == true))//正式的去掉
	{
		string strOldXml = strFileName;
		wstring wstrPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
		string strPostfix = utf16_to_gbk(wstrPostfix);
		replace_all_distinct(strOldXml,strPostfix.c_str(),".xml",0);
		if(strFileName.find(m_strCfgTempPath.c_str())!=string::npos)
		{
			replace_all_distinct(strOldXml,m_strCfgTempPath.c_str(),m_strSrcPath.c_str(),0);
		}
//		ForceDeleteFile(strXml);//临时注释掉
		CopyFile(strOldXml.c_str(), strXml.c_str(), true);
		vector<string> vecChild;
		vecChild.push_back(strXml);
		vecChild.push_back(strOldXml);
		vecChild.push_back(strTxt);
		CRecordManager::GetInst()->AddFileRecord(strFileName);
		CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
		for (uint32 i=0;i<vecChild.size();++i)
		{
			CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
		}
	}
	else if(m_Convert == true)
	{
		vector<bool> vectype;
		GetColType(strFileName,vectype);
		WriteToXml(strXml,vectype,strFileName);
		vector<string> vecChild;
		vecChild.push_back(strXml);
		vecChild.push_back(strTxt);
		CRecordManager::GetInst()->AddFileRecord(strFileName);
		CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
		for (uint32 i=0;i<vecChild.size();++i)
		{
			CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
		}
	}
	if((m_Convert==false)&&(strXml.find("chinese_simple") == string::npos))
	{
		string strOldXml = strFileName;
		wstring wstrPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	


		wstring wstrCppExtend = CSplitInfo::GetInst()->WGetCppCfgExtend();//临时注释掉

		string strPostfix = utf16_to_gbk(wstrPostfix);

		string strCppExtend = utf16_to_gbk(wstrCppExtend);//临时注释掉

		replace_all_distinct(strOldXml,strPostfix.c_str(),".xml",0);

		replace_all_distinct(strOldXml,strCppExtend.c_str(),".xml",0);//临时注释掉
		if(strFileName.find(m_strCfgTempPath.c_str())!=string::npos)
		{
			replace_all_distinct(strOldXml,m_strCfgTempPath.c_str(),m_strSrcPath.c_str(),0);
		}
		ForceDeleteFile(strXml);//临时注释掉
		TiXmlDocument * pDoc = new TiXmlDocument();
		bool isLink = false;
		if(pDoc->LoadFile(strOldXml))
		{
			if(pDoc->RootElement()->FirstChild("link"))
			{
				string path;
				int pos = strOldXml.find_last_of("/");
				path = strOldXml.substr(0,pos);
				string str = pDoc->RootElement()->FirstChild("link")->ToElement()->GetText();
				int spos=-1;
				while(true)
				{
					string temp = str.substr(0,3);
					if(temp=="../")
					{
						str = str.substr(3,str.size());
						pos = path.find_last_of("/");
						path = path.substr(0,pos);
						continue;
					}
					spos=str.find_first_of("/",0);
					if(spos!=-1)
					{
						path=path+"/"+str.substr(0,spos);
						str = str.substr(spos+1,str.size());
						continue;
					}
					path=path+"/"+str;
					break;
				}
				CopyFile(path.c_str(),strXml.c_str(),true);
				isLink = true;
			}
		}
		delete pDoc;
		pDoc = NULL;
		if(isLink==false)
		{
			CopyFile(strOldXml.c_str(), strXml.c_str(), true);
		}
		vector<string> vecChild;
		vecChild.push_back(strXml);
		vecChild.push_back(strOldXml);
		vecChild.push_back(strTxt);
		CRecordManager::GetInst()->AddFileRecord(strFileName);
		CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
		for (uint32 i=0;i<vecChild.size();++i)
		{
			CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
		}
	}
	else if(m_Convert==false)
	{
		vector<bool> vectype;
		GetColType(strFileName,vectype);
		WriteToXml(strXml,vectype,strFileName);
		vector<string> vecChild;
		vecChild.push_back(strXml);
		vecChild.push_back(strTxt);
		CRecordManager::GetInst()->AddFileRecord(strFileName);
		CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
		for (uint32 i=0;i<vecChild.size();++i)
		{
			CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

string CDataType2Xml::GetXmlPath(string& strFileName)
{
	string strResult;
	const string& strSearch = CSplitInfo::GetInst()->GetLangSrcFolder();
	int pos = strFileName.find(strSearch.c_str());
	string strPostfix = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strCppPostfix = CSplitInfo::GetInst()->GetCppCfgExtend();
	if (strFileName.find(strCppPostfix.c_str()) != -1)
		strPostfix = strCppPostfix;
	if (pos != -1)
	{
		strResult = strFileName.substr(pos+strSearch.length(),strFileName.length()-pos-1);
		strResult = m_pSplitInfo->GetLangTablePath() + strResult;
		replace_all_distinct(strResult,strPostfix.c_str(),".xml",0);
	}
	else
	{
		strResult = strFileName;
		if(m_Convert==false)
		{
			if(strResult.find(m_strCfgTempPath.c_str())==string::npos)
			{
				const string& strSrc = CSplitInfo::GetInst()->GetSrcFolder();
				const string& strTgt = CSplitInfo::GetInst()->GetTgtFolder();
				replace_all_distinct(strResult,strSrc,strTgt,0);
			}
			else
			{
				replace_all_distinct(strResult,m_strCfgTempPath,m_strTgtPath.c_str(),0);
			}
		}
		replace_all_distinct(strResult,strPostfix,".xml",0);
	}
	return strResult;
}

bool CDataType2Xml::BeMultiRowTable(string& strFileName)
{
	int pos = strFileName.find(CSplitInfo::GetInst()->GetLangSrcFolder());
	if (pos != -1)
		return false;
	string strCppExtend = CSplitInfo::GetInst()->GetCppCfgExtend();
	string strLuaExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	if (strCppExtend != strLuaExtend && strFileName.find(strCppExtend.c_str()) != -1)
		return false;
	CDifInfo difInfo(strFileName.c_str());
	return difInfo.BeMultiRowTable();
}

void	CDataType2Xml::GetColType(string& strFileName,vector<bool>& vecType)
{
	COriFileOperate oriFile(strFileName);
	string strExtend = CSplitInfo::GetInst()->GetCppCfgExtend();
	if (!m_pSplitInfo->GetBeKeepBlankLine(strFileName))
	{
		oriFile.CleanBlanRow();
		oriFile.CleanNoteRow();
	}
	int32 nRowNum = oriFile.GetRowCount();
	int32 nColNum = 0;
	nColNum = oriFile.GetColCountByRow(0);
	vecType.clear();
	for (int i=0;i<nColNum;++i)
	{
		vecType.push_back(true);
	}
	for (int i = 1; i < nRowNum; i++)
	{
		for (int j = 0; j < nColNum; j++)
		{
			if (!oriFile.BeNumber(i,j))
			{
				vecType[j] = false;
			}
		}
	}
}

void CDataType2Xml::WriteToXml(string& strXml, vector<bool>& vecType,string& FileName)
{
	COriFileOperate oriFile(FileName);
	int nVecSize = vecType.size();
	const string& strOrilang = CSplitInfo::GetInst()->GetOriLang();
	if (strXml.find(strOrilang) != -1)
	{
		for (int i=1;i<nVecSize;++i)
		{
			vecType[i] = false;
		}
	}
	char* szNumOrStrInfo = new char[nVecSize+1];
	for (int i = 0; i < nVecSize; i++)
	{
		if (vecType[i])
		{
			szNumOrStrInfo[i] = 'n';
		}
		else
		{
			szNumOrStrInfo[i] = 's';
		}
	}
	szNumOrStrInfo[nVecSize] = '\0';
	ForceDeleteFile(strXml);
	TiXmlDocument* pXmlDoc = new TiXmlDocument;
	pXmlDoc->InsertEndChild(TiXmlElement("config"));
	TiXmlNode* pXmlNode = pXmlDoc->FirstChild("config");
	pXmlNode->InsertEndChild(TiXmlElement("TableType"));
	TiXmlElement* pXmlElem = pXmlNode->FirstChildElement("TableType");
	pXmlElem->InsertEndChild(TiXmlText("S"));

//	pXmlNode->InsertEndChild(TiXmlElement("FieldType"));
//	pXmlElem = pXmlNode->FirstChildElement("FieldType");
//	pXmlElem->InsertEndChild(TiXmlText(szNumOrStrInfo));
	
	string str = szNumOrStrInfo;
	string str1 = oriFile.GetDataByRowCol(0,0);
	transform(str1.begin(),str1.end(),str1.begin(),toupper);
	for(uint32 i=1;i<=str.size();i++)
	{
		TiXmlElement* cxn = new TiXmlElement("Col");
		pXmlNode->LinkEndChild(cxn);
	//	cxn->SetAttribute("ColNum",i);
		if(str1!="NOTE:")
		{
			cxn->SetAttribute("Name",oriFile.GetDataByRowCol(0,i-1).c_str());
		}
		else
		{
			cxn->SetAttribute("Name",oriFile.GetDataByRowCol(1,i-1).c_str());
		}
		string ss;
		stringstream temp;
		temp<<szNumOrStrInfo[i-1];
		temp>>ss;
		cxn->SetAttribute("Type",ss);
	}
    
	pXmlDoc->SaveFile(strXml);

	delete pXmlDoc;
	pXmlDoc = NULL;
	delete[] szNumOrStrInfo;
	szNumOrStrInfo = NULL;
}

void CDataType2Xml::ForceDeleteFile(const string& strFile)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strFile.c_str(),&fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return ;
	string strFullPath = strFile;
	strFullPath.resize(strFullPath.length()+1);
	SHFILEOPSTRUCT op;
	memset(&op,0,sizeof(op));
	op.pFrom = strFullPath.c_str();
	op.pTo = NULL;
	op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	op.wFunc = FO_DELETE;
	op.hwnd = NULL;
	op.hNameMappings = NULL;
	if (SHFileOperation(&op))
	{
		string strMessage = strFile + "需要更新内容，但现存文件不能删除，请确认是否有其他程序在使用中";
		MessageBox(0,strMessage.c_str(),"错误",NULL);
		exit(0);
	}
}

void CDataType2Xml::WriteToXml(const char* szFileName, const char* szReplace,string& FileName)
{
	COriFileOperate oriFile(FileName);
	assert(szFileName);
	string szDifFileName = szFileName;
	const string& strSrc = CSplitInfo::GetInst()->GetSrcFolder();
	const string& strTgt = CSplitInfo::GetInst()->GetTgtFolder();
	replace_all_distinct(szDifFileName, ".xml", szReplace, 0);
	replace_all_distinct(szDifFileName, strTgt, strSrc, 0);

	SetDataTypeVector(szDifFileName.c_str());
	int nVecSize = m_vecDataType.size();
	char* szNumOrStrInfo = new char[nVecSize+1];
	for (int i = 0; i < nVecSize; i++)
	{
		if (m_vecDataType[i])
		{
			szNumOrStrInfo[i] = 'n';
		}
		else
		{
			szNumOrStrInfo[i] = 's';
		}
	}
	szNumOrStrInfo[nVecSize] = '\0';
	TiXmlDocument* pXmlDoc = new TiXmlDocument;
	if (pXmlDoc->LoadFile(szFileName))
	{
		pXmlDoc->Clear();
	}
	pXmlDoc->InsertEndChild(TiXmlElement("config"));
	TiXmlNode* pXmlNode = pXmlDoc->FirstChild("config");
	pXmlNode->InsertEndChild(TiXmlElement("TableType"));
	TiXmlElement* pXmlElem = pXmlNode->FirstChildElement("TableType");
	pXmlElem->InsertEndChild(TiXmlText("S"));

//	pXmlNode->InsertEndChild(TiXmlElement("FieldType"));
//	pXmlElem = pXmlNode->FirstChildElement("FieldType");
//	pXmlElem->InsertEndChild(TiXmlText(szNumOrStrInfo));
	string str = szNumOrStrInfo;
	string str1 = oriFile.GetDataByRowCol(0,0);
	transform(str1.begin(),str1.end(),str1.begin(),toupper);
	for(uint32 i=1;i<=str.size();i++)
	{
		TiXmlElement* cxn = new TiXmlElement("Col");
		pXmlNode->LinkEndChild(cxn);
	//	cxn->SetAttribute("ColNum",i);
		if(str1!="NOTE:")
		{
			cxn->SetAttribute("Name",oriFile.GetDataByRowCol(0,i-1).c_str());
		}
		else
		{
			cxn->SetAttribute("Name",oriFile.GetDataByRowCol(1,i-1).c_str());
		}
		string ss;
		stringstream temp;
		temp<<szNumOrStrInfo[i-1];
		temp>>ss;
		cxn->SetAttribute("Type",ss);
	}
	pXmlDoc->SaveFile(szFileName);

	delete pXmlDoc;
	pXmlDoc = NULL;
	delete[] szNumOrStrInfo;
	szNumOrStrInfo = NULL;
}

void CDataType2Xml::SetDataTypeVector(const char* szFileName)
{
	string strTemp = szFileName;
	COriFileOperate oriFile(strTemp);
	oriFile.CleanBlanRow();
	oriFile.CleanNoteRow();
	//for (int i=0;i<oriFile.GetRowCount();++i)
	//{
	//	m_vecElementInfo.push_back(vector<string>());
	//	for (int j=0;j<oriFile.GetColCountByRow(0);++j)
	//	{
	//		m_vecElementInfo.back().push_back(oriFile.GetDataByRowCol(i,j));
	//	}
	//}
	int32 nRowNum = oriFile.GetRowCount();
	int32 nColNum = 0;
	if (nRowNum > 0)
	{
		nColNum = oriFile.GetColCountByRow(0);
	}
	m_vecDataType.clear();
	for (int nCol = 0; nCol < nColNum; nCol++)
	{
		m_vecDataType.push_back(true);
	}
	
	for (int i = 1; i < nRowNum; i++)
	{
		for (int j = 0; j < nColNum; j++)
		{
			if (!oriFile.BeNumber(i,j))
			{
				m_vecDataType[j] = false;
			}
		}
	}
}

bool CDataType2Xml::BeInDataType2Xml(const char* szFileName)
{
	if (m_pSplitInfo->BeInSplitList(szFileName))
	{
		return true;
	}
	else
	{
		return false;
	}
}

#include "stdafx.h"
#include "CAdjustGermanLan.h"
#include "common.h"
#include "CXmlConfig.inl"
#include "CSplitInfo.h"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

//#define CLEANGERMAN_COPYXML

CAdjustGermanLan::CAdjustGermanLan()
:m_strNote("_note")
{
	ReadMappedXML();
	//m_strPath = "../../cfg/data/src/language/german/";
	//m_wstrFilePath = L"/sphira_gameforge_batch1_04.05.2011/";
}

CAdjustGermanLan::~CAdjustGermanLan()
{

}

void CAdjustGermanLan::StartProcess()
{
	m_strPath = CSplitInfo::GetInst()->GetLanguageTempPath() + "/"
		+ CSplitInfo::GetInst()->GetForeLang() + "/";
	m_wstrFilePath = CSplitInfo::GetInst()->WGetTransImportPath();
	FileTreeWalk(m_wstrFilePath.c_str(), CleanLangFile, NULL);
#ifdef CLEANGERMAN_COPYXML
	string strXmlPath = CSplitInfo::GetInst()->GetLangTablePath();
	wstring wstrXml = utf8_to_utf16(strXmlPath);
	FileTreeWalk(wstrXml.c_str(), WalkXml, NULL);
#endif
}

FTW_RESULT CAdjustGermanLan::CleanLangFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	if(strFileName.find("/", 3) == string::npos)
	{
		return _FTW_CONTINUNE;
	}
	if (strFileName.find(CSplitInfo::GetInst()->GetTransExportPath()) != -1)
	{
		return _FTW_CONTINUNE;
	}
	CAdjustGermanLan* pManager = CAdjustGermanLan::GetInst();
	pManager->ChangeLanFormat(strFileName);
	return _FTW_CONTINUNE;
}

void CAdjustGermanLan::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	inFile.seekg(3);
	string str;
	while(getline(inFile,str))
	{
		if (str.find(m_strNote.c_str()) != -1)
		{
			continue;
		}
		vecRef.push_back(vector<string>());
		int pos = -1;
		if(str.find('\t') == 0)
		{
			string strMsg = "配置表名为:" + strFileName + "\t内容:" + str;
			if(GetLogToFile())
			{
				string strMsgAll = "配置表格式错误,第一列名为空" + strMsg;
				PrintLog(strMsgAll);
			}
			else
			{
				GenErr("配置表格式错误,第一列名为空",strMsg.c_str());
			}
		}
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

void CAdjustGermanLan::ReadUtf16Txt(string& strFileName,vector<vector<wstring> >& vecRead)
{
	ifstream inFile(strFileName.c_str(),ios_base::binary | ios_base::in);
	wchar_t wcharTemp;
	wstring wstr;
	//vector<vector<wstring> > vecRead;
	vecRead.push_back(vector<wstring>());
	inFile.seekg(2,ios::beg);
	bool bInQuot = false;
	while (inFile.peek() != EOF)
	{
		inFile.read((char*)(&wcharTemp),2);
		if (wcharTemp == 0x0009 && !bInQuot)
		{
			if (vecRead[0].empty() && wstr.empty())
			{
				string strMsg = "配置表名为:" + strFileName;
				if(GetLogToFile())
				{
					string strMsgAll = "配置表格式错误,第一列名为空" + strMsg;
					PrintLog(strMsgAll);
				}
				else
				{
					GenErr("配置表格式错误,第一列名为空",strMsg.c_str());
				}
			}
			vecRead.back().push_back(wstr);
			wstr = L"";
		}
		else if (wcharTemp == 0x000D)
		{
			vecRead.back().push_back(wstr);
			wstr = L"";
			inFile.seekg(2,ios::cur);
			vecRead.push_back(vector<wstring>());
			bInQuot = false;
		}
		else if (wcharTemp == 0x0022)
		{
			wstr.append(1,wcharTemp);
			bInQuot = !bInQuot;
		}
		else
		{
			wstr.append(1,wcharTemp);
		}
	}
	inFile.close();
}

void CAdjustGermanLan::ReadUtf16Txt(string& strFileName,vector<vector<string> >& vecRef)
{
	vector<vector<wstring> > vecWData;
	ReadUtf16Txt(strFileName,vecWData);
	for (int i=0;i<vecWData.size();++i)
	{
		vecRef.push_back(vector<string>());
		for (int j=0;j<vecWData[i].size();++j)
		{
			string strContent  = utf16_to_utf8(vecWData[i][j]);
			vecRef[i].push_back(strContent);
		}
	}
}

void CAdjustGermanLan::ChangeLanFormat(string& strFileName)
{
	//if (!BeUtf16Format(strFileName))
	//{
	//	return;
	//}
	//if(strFileName.find("GUIStaticText.txt") != string::npos)
	//{
	//	int i = 0;
	//}
	vector<vector<string> > vecData;
	switch(UtfFormatNumber(strFileName))
	{
	case 8:
		ReadUtf8Txt(strFileName,vecData);
		break;
	case 16:
		ReadUtf16Txt(strFileName,vecData);
		break;
	default:
		return;
	}
	RemoveNoteCol(strFileName, vecData);
	string strGui = "/" + CSplitInfo::GetInst()->GetGuiFolder() + "/";
	if(strFileName.find(strGui) == string::npos)
	{
		for (int i=1;i<(int)vecData.size();++i)
		{
			if (vecData[i].empty())
			{
				continue;
			}
			string& strIndex = vecData[i][0];
			ReplaceKeyName(strIndex);
		}
	}
	string strSavePath = GetRelativePath(strFileName);
	strSavePath = m_strPath + strSavePath;
	CreateFileDirectory(strSavePath);
	ofstream outFile(strSavePath.c_str());
	size_t uVecData0Size = vecData[0].size();
	size_t iVecData0Size = (int) uVecData0Size;
	for(int j = 0; j < iVecData0Size; ++j)
	{
		if(vecData[0][j].find(' ') != string::npos)
		{
			stringstream strMsg;
			strMsg << "配置表名为:" << strFileName << " 标题行第" << j << "列标题[" << vecData[0][j] << "]含有空格";
			if(GetLogToFile())
			{
				PrintLog(strMsg.str(), true);
			}
			printf("%s\n",strMsg.str().c_str());
			ClearSpace(vecData[0][j]);
		}
	}
	for (int i=0;i<(int)vecData.size();++i)
	{
		size_t uVecDataISize = vecData[i].size();
		if(uVecDataISize > uVecData0Size)
		{
			stringstream strMsg;
			strMsg << "配置表名为:" << strFileName << " 第" << i << "行列数" << uVecDataISize << "超出标题列数" << uVecData0Size;
			if(GetLogToFile())
			{
				string strMsgAll = "配置表格式错误,列数过多：" + strMsg.str();
				PrintLog(strMsgAll);
			}
			else
			{
				GenErr("配置表格式错误,列数过多",strMsg.str().c_str());
			}
		}
		for (int j=0; j < iVecData0Size; ++j)
		{
			if (vecData[i].empty() || vecData[i][0] == "")
			{
				continue;
			}
			if(j < uVecDataISize)
			{
				outFile << vecData[i][j];
			}
			if(j == iVecData0Size - 1)
			{
				outFile << endl;
			}
			else
			{
				outFile << "\t";
			}
		}
	}
	outFile.close();
}

void CAdjustGermanLan::ReplaceKeyName(string& str)
{
	map<string,string>::iterator iter = m_mapName.begin();
	for (;iter != m_mapName.end();++iter)
	{
		if (str.find(iter->second.c_str()) != -1)
		{
			string strReplace = iter->first;
			strReplace = mbcs_to_MemH64(strReplace);
			replace_all_distinct(str,iter->second,strReplace,0);
			return;
		}
	}
}

void CAdjustGermanLan::RemoveNoteCol(const string& strFileName, vector<vector<string> >& vecRef)
{
	vector<string>& vecFirstRow = vecRef[0];
	vector<int> vecNote;
	for (int i=(int)vecFirstRow.size()-1;i>0;--i)
	{
		if (vecFirstRow[i].find("note") != -1)
		{
			vecNote.push_back(i);
		}
	}
	for (int i=0;i<vecNote.size();++i)
	{
		for (int j=0;j<vecRef.size();++j)
		{
			if(vecRef[j].empty())
				continue;
			if(vecNote[i] + 1 > vecRef[j].size())
			{
				cout << "文件[" << strFileName << "] 第[" << (i + 1) << "]行只有[" << vecRef[j].size() << "]列，第[" << (vecNote[i] + 1) << "]列不存在\n";
			}
			else
			{
				vecRef[j].erase(vecRef[j].begin()+vecNote[i]);
			}
		}
	}
}

bool CAdjustGermanLan::ReadMappedXML()
{
	string strCfg = "NameCompare.xml";
	ifstream config(strCfg.c_str());
	if(!config)
		return false;

	CXmlConfig *pXmlCfg = new CXmlConfig();
	pXmlCfg->LoadConfig( "root" , config );
	config.close();

	TiXmlNode* pTableNode = pXmlCfg->Get<TiXmlElement*>("table");
	if( pTableNode == NULL )
	{
		delete pXmlCfg;
		return false; 
	}
	for(TiXmlNode* pNode = pTableNode->FirstChild("entry");pNode;pNode=pNode->NextSibling("entry"))
	{
		TiXmlElement* pElement = pNode->ToElement();
		string strName = pElement->Attribute("Name");
		string strMap = pElement->Attribute("MappedName");
		m_mapName.insert(make_pair(strName,strMap));
	}
	return true;
}

string CAdjustGermanLan::GetRelativePath(string& str)
{
	string strFind = utf16_to_gbk(m_wstrFilePath);
	int pos = str.find(strFind.c_str());
	string strResult = str.substr(pos+strFind.length(),str.length()-pos-strFind.length());
	int pos2= strResult.rfind("/");
	if(pos2 != string::npos)
	{
		string strPath = strResult.substr(0,pos2);
		transform(strPath.begin(),strPath.end(),strPath.begin(),tolower);
		strResult = strResult.substr(pos2,strResult.length()-pos2);
		strResult = strPath + strResult;
	}
	return strResult;
}

bool CAdjustGermanLan::BeUtf8Format(string& strFileName)
{
	ifstream inFile(strFileName.c_str(),ios_base::binary | ios_base::in);
	if (!inFile)
		return false;
	unsigned char head[3];
	inFile.read((char*)head,3);
	inFile.close();
	bool bResult = false;
	if (head[0] == 0xEF && head[1] == 0xBB && head[2] == 0xBF)
		bResult =  true;
	if (!bResult)
	{
		ofstream outLog("log.txt",ios_base::app);
		outLog<<strFileName<<endl;
	}
	return bResult;
}

bool CAdjustGermanLan::BeUtf16Format(string& strFileName)
{
	ifstream inFile(strFileName.c_str(),ios_base::binary | ios_base::in);
	if (!inFile)
		return false;
	unsigned char head[2];
	inFile.read((char*)head,2);
	inFile.close();
	bool bResult = false;
	if (head[0] == 0xFF && head[1] == 0xFE)
		bResult =  true;
	if (!bResult)
	{
		ofstream outLog("log.txt",ios_base::app);
		outLog<<strFileName<<endl;
	}
	return bResult;
}

int CAdjustGermanLan::UtfFormatNumber(string& strFileName)
{
	ifstream inFile(strFileName.c_str(),ios_base::binary | ios_base::in);
	if (!inFile)
		return 0;
	unsigned char head[3];
	inFile.read((char*)head,3);
	inFile.close();
	int bResult = 0;
	if (head[0] == 0xFF && head[1] == 0xFE)
		bResult =  16;
	else if (head[0] == 0xEF && head[1] == 0xBB && head[2] == 0xBF)
		bResult =  8;
	if (bResult != 16)
	{
		ofstream outLog("log.txt",ios_base::app);
		outLog<<strFileName<< " " << bResult << endl;
		
		if(GetLogToFile())
		{
			stringstream strExpLog;
			strExpLog << strFileName<< " " << bResult;
			PrintLog(strExpLog.str(), true);
		}
	}
	return bResult;
}

FTW_RESULT CAdjustGermanLan::WalkXml( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".xml" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	const string& strChinese = CSplitInfo::GetInst()->GetOriLang();
	if (strFileName.find(strChinese) == -1)
	{
		return _FTW_CONTINUNE;
	}
	CAdjustGermanLan* pManager = CAdjustGermanLan::GetInst();
	pManager->CopyXml_Geramn(strFileName);
	return _FTW_CONTINUNE;
}

void CAdjustGermanLan::CopyXml_Geramn(string& strFileName)
{
	string strSave = strFileName;
	replace_all_distinct(strSave, CSplitInfo::GetInst()->GetOriLang(),
		CSplitInfo::GetInst()->GetForeLang(), 0);
	ForceDeleteFile(strSave);
	CreateFileDirectory(strSave);
	CopyFile(strFileName.c_str(), strSave.c_str(), false);
}

void CAdjustGermanLan::ForceDeleteFile(const string& strFile)
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
		if(GetLogToFile())
		{
			PrintLog(strMessage);
		}
		else
		{
			MessageBox(0,strMessage.c_str(),"错误",NULL);
		}
		exit(0);
	}
}


#pragma warning(pop)

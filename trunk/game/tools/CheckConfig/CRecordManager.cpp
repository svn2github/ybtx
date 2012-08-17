#include "stdafx.h"
#include "CRecordManager.h"
#include "CXmlConfig.inl"
#include "PkgStream.h"
#include "CodeCvs.h"

CRecordManager* CRecordManager::GetInst()
{
	static CRecordManager Inst;
	return &Inst;
}
bool CRecordManager::GetFileAmendTime(string strFile,FILETIME* time)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strFile.c_str(),&fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	*time = fd.ftLastWriteTime;
	FindClose(hFind);
	return true;
}

void CRecordManager::SaveRecordEntry()
{
	FILE* PFile = fopen("Record.data","wb");
	if (!PFile)
	{
		string error = "Record.data 打开失败";
		MessageBox(NULL,error.c_str(),NULL,0);
		return;
	}
	int fileNum = (int)m_vecRecord.size();
	fwrite((TCHAR*)&fileNum,sizeof(int),1,PFile);
	for (int i=0;i<fileNum;++i)
	{
		string& strFunction = m_vecRecord[i].m_strFunction;
		int nameSize = (int)strFunction.size();
		fwrite((TCHAR*)&nameSize,sizeof(int),1,PFile);
		fwrite(strFunction.c_str(),strFunction.length(),1,PFile);
		int containFNum = (int)m_vecRecord[i].m_mapFileTime.size();
		fwrite((TCHAR*)&containFNum,sizeof(int),1,PFile);
		for (map<string,FILETIME>::iterator iter = m_vecRecord[i].m_mapFileTime.begin();iter!=m_vecRecord[i].m_mapFileTime.end();++iter)
		{
			string strFileName = iter->first;
			FILETIME& ft = iter->second;
			string strPath = m_vecRecord[i].m_mapFilePath[strFileName];
			int fnSize = (int)strFileName.size();
			int ptSize = (int)strPath.size();
			fwrite((TCHAR*)&fnSize,sizeof(int),1,PFile);
			fwrite(strFileName.c_str(),strFileName.size(),1,PFile);
			fwrite((TCHAR*)&ptSize,sizeof(int),1,PFile);
			fwrite(strPath.c_str(),strPath.size(),1,PFile);
			fwrite((TCHAR*)&ft.dwLowDateTime,sizeof(DWORD),1,PFile);
			fwrite((TCHAR*)&ft.dwHighDateTime,sizeof(DWORD),1,PFile);
		}
		fwrite((TCHAR*)&m_vecRecord[i].m_bOK,sizeof(bool),1,PFile);
	}
	fclose(PFile);
}

void CRecordManager::ReadRecordDoc()
{
	if (!m_bFastCheck)
		return;
	FILE* PFile = fopen("Record.data","rb");
	if (!PFile)
	{
		return;
	}
	SQR_TRY
		int recordNum=0;
	fread((TCHAR*)&recordNum,sizeof(int),1,PFile);
	for (int i=0;i<recordNum;++i)
	{
		RecordData data;
		string strFunction;
		int nameSize=0;
		fread((TCHAR*)&nameSize,sizeof(int),1,PFile);
		strFunction.resize(nameSize);
		fread((TCHAR*)strFunction.c_str(),nameSize,1,PFile);
		data.m_strFunction = strFunction;
		int containFNum;
		fread((TCHAR*)&containFNum,sizeof(int),1,PFile);
		for (int j=0;j<containFNum;++j)
		{
			int fnSize;
			fread((TCHAR*)&fnSize,sizeof(int),1,PFile);
			string strFileName;
			strFileName.resize(fnSize);
			fread((TCHAR*)strFileName.c_str(),fnSize,1,PFile);
			int pathSize;
			fread((TCHAR*)&pathSize,sizeof(int),1,PFile);
			string strPath;
			strPath.resize(pathSize);
			fread((TCHAR*)strPath.c_str(),pathSize,1,PFile);
			FILETIME ft;
			fread((TCHAR*)&ft.dwLowDateTime,sizeof(DWORD),1,PFile);
			fread((TCHAR*)&ft.dwHighDateTime,sizeof(DWORD),1,PFile);
			data.m_mapFileTime.insert(make_pair(strFileName,ft));
			data.m_mapFilePath.insert(make_pair(strFileName,strPath));
		}
		fread((TCHAR*)&data.m_bOK,sizeof(bool),1,PFile);
		m_vecRecord.push_back(data);
	}
	SQR_CATCH(exp)
	{
		string strMsg = "读取转换文档信息出错";
		strMsg = strMsg + exp.ErrorTitle();
		m_vecRecord.clear();
		fclose(PFile);
	}
	SQR_TRY_END
		fclose(PFile);
}

bool CRecordManager::BeNeedCheck(string strFunName)
{
	if (!BeInRecordList(strFunName))
		return true;
	int index;
	FindRecordData(strFunName,index);
	if(m_vecRecord[index].m_bOK)
		return false;
	else
		return true;
}

void CRecordManager::SetRecordState(string strFunName,bool bState)
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (strFunName == m_vecRecord[i].m_strFunction)
		{
			m_vecRecord[i].m_bOK = bState;
			if (bState)
			{
				SaveCurrentTime(strFunName);
			}
			break;
		}
	}
	if(bState)
		SaveRecordEntry();
}

bool CRecordManager::BeInRecordList(string& strFunName)
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (strFunName == m_vecRecord[i].m_strFunction)
		{
			return true;
		}
	}
	return false;
}

bool CRecordManager::BeNoneRecord()
{
	return m_vecRecord.empty();
}

void CRecordManager::ReadXmlCfg()
{
	if (!m_mapFunction.empty())
		return;

	ipkgstream file_strm(L"", "config.xml");
	if (!file_strm)
		GenErr("Can't open config.xml");

	CXmlConfig *pXmlCfg = new CXmlConfig( "root" , file_strm );
	file_strm.close();
	
	TiXmlNode* pRootNode = pXmlCfg->GetRootNode();
	TiXmlNode* pConfig = pRootNode->FirstChild("config");
	m_strCfgPath = gbk_to_utf16(pConfig->FirstChild("CfgPath")->ToElement()->Attribute("path"));
	m_strLanguage = pConfig->FirstChild("Language")->ToElement()->Attribute("name");
	string strFastCheck = pConfig->FirstChild("FastCheck")->ToElement()->Attribute("turn_on");
	if (strFastCheck == "yes")
		m_bFastCheck = true;
	else
		m_bFastCheck = false;
	
	for (TiXmlNode* pSection = pRootNode->FirstChild("Function_Section");pSection;pSection = pSection->NextSibling("Function_Section"))
	{	
		TiXmlElement* pNameEle = pSection->FirstChild("FunctionName")->ToElement();
		string strName = pNameEle->Attribute("name");
		if (!m_mapFunction.count(strName))
			m_mapFunction[strName] = vector<string>();
		for (TiXmlNode* pRelatinFile = pSection->FirstChild("RelationFile");pRelatinFile;pRelatinFile = pRelatinFile->NextSibling("RelationFile"))
		{
			TiXmlElement* pFileNameNode = pRelatinFile->ToElement();
			string strFileName = pFileNameNode->Attribute("name");
			int iBegin; 
			int iEnd;
			pFileNameNode->QueryIntAttribute("begin",&iBegin);
			pFileNameNode->QueryIntAttribute("end",&iEnd);
			if (iBegin!=iEnd)
			{
				for (int i=iBegin;i<=iEnd;++i)
				{
					char numArray[10];
					_itoa(i,numArray,10);
					string strReal = strFileName + numArray;
					m_mapFunction[strName].push_back(strReal);
				}
			}
			else
				m_mapFunction[strName].push_back(strFileName);
		}
		for (TiXmlNode* pDependence = pSection->FirstChild("Depedence");pDependence;pDependence = pDependence->NextSibling("Depedence"))
		{
			string strDFun = pDependence->ToElement()->Attribute("name");
			if (!m_mapDepedence.count(strName))
				m_mapDepedence[strName] = vector<string>();
			m_mapDepedence[strName].push_back(strDFun);
		}
	}

	delete pXmlCfg;
	pXmlCfg = NULL;
}

void CRecordManager::CalculateState()
{
	ReadXmlCfg();
	ReadRecordDoc();
	UpdateRecordData();
	FileTreeWalk(m_strCfgPath.c_str(),CheckFilePath,NULL);
	CheckAllFileExist();
	CheckFileTime();
	CheckDependenceFun();
	SetCommonDependece();
}

void CRecordManager::UpdateFilePath(string& strFileName)
{
	if (strFileName.find(".txt") != string::npos)
	{
		return;
	}
	if (strFileName.find(".xml") != string::npos)
	{
		return;
	}

	int pos = (int)strFileName.rfind("/");
	int periodPos = (int)strFileName.find(".",pos);
	string strName = strFileName.substr(pos+1,periodPos-pos-1);
	string strPath = strFileName.substr(0,pos);
	if (strPath.find("language") != -1)
		return;
	if (strPath.find("lang") != -1 && strPath.find(m_strLanguage) == -1)
		return;
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if(m_vecRecord[i].m_mapFilePath.find(strName) != m_vecRecord[i].m_mapFilePath.end())
		{
			if (strFileName != m_vecRecord[i].m_mapFilePath[strName])
			{
				m_vecRecord[i].m_mapFilePath[strName] = strFileName;
				m_vecRecord[i].m_bOK = false;
			}
			if (find(m_vecRecord[i].m_vecRefresh.begin(),m_vecRecord[i].m_vecRefresh.end(),strName) != m_vecRecord[i].m_vecRefresh.end())
			{
				cout<<strName<<"存在重复文件！"<<endl;
			}
			m_vecRecord[i].m_vecRefresh.push_back(strName);
		}
	}
}

void CRecordManager::CheckFileTime()
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (m_vecRecord[i].m_bOK)
		{
			for (map<string,FILETIME>::iterator iter = m_vecRecord[i].m_mapFileTime.begin();iter!=m_vecRecord[i].m_mapFileTime.end();++iter)
			{
				string strFileName = iter->first;
				FILETIME ft;
				if(GetFileAmendTime(m_vecRecord[i].m_mapFilePath[strFileName],&ft))
				{
					if (ft.dwHighDateTime != iter->second.dwHighDateTime || ft.dwLowDateTime != iter->second.dwLowDateTime)
					{
						m_vecRecord[i].m_bOK = false;
						break;
					}
				}
				else
				{
					m_vecRecord[i].m_bOK = false;
					break;
				}
			}
		}
	}
}

void CRecordManager::UpdateRecordData()
{
	map<string,vector<string> >::iterator iter = m_mapFunction.begin();
	map<string,vector<string> >::iterator iterEnd = m_mapFunction.end();
	for (;iter!=iterEnd;++iter)
	{
		bool bSame = true;
		vector<string>& vecString = iter->second;
		string strFunction = iter->first;
		RecordData data;
		data.m_strFunction = strFunction;
		data.m_bOK = false;
		FILETIME ft;
		ft.dwHighDateTime = 0;
		ft.dwLowDateTime = 0;
		for (int j=0;j<(int)vecString.size();++j)
		{
			data.m_mapFileTime.insert(make_pair(vecString[j],ft));
			data.m_mapFilePath.insert(make_pair(vecString[j],string()));
		}
		if(BeInRecordList(strFunction))
		{
			int index;
			FindRecordData(strFunction,index);
			if(vecString.size() == m_vecRecord[index].m_mapFileTime.size())
			{
				for (int i=0;i<(int)vecString.size();++i)
				{
					if (m_vecRecord[index].m_mapFileTime.find(vecString[i]) == m_vecRecord[index].m_mapFileTime.end())
					{
						m_vecRecord[index] = data;
						break;
					}
				}
			}
			else
			{
				m_vecRecord[index] = data;
			}
		}
		else
		{
			m_vecRecord.push_back(data);
		}
	}
}

void CRecordManager::FindRecordData(string& strFunction,int& index)
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (strFunction == m_vecRecord[i].m_strFunction)
		{
			index = i;
			return;
		}
	}
}

FTW_RESULT CRecordManager::CheckFilePath( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = (int)wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".lua" ) &&  _wcsicmp( szFileName + nLen - 4, L".txt" ))
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CRecordManager::GetInst()->UpdateFilePath(strFileName);
	return _FTW_CONTINUNE;
}

void CRecordManager::CheckAllFileExist()
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (m_vecRecord[i].m_vecRefresh.size() != m_vecRecord[i].m_mapFilePath.size())
		{
			m_vecRecord[i].m_bOK = false;
		}
	}
}

void CRecordManager::SaveCurrentTime(string strFunName)
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (m_vecRecord[i].m_strFunction == strFunName)
		{
			for (map<string,FILETIME>::iterator iter = m_vecRecord[i].m_mapFileTime.begin();iter!=m_vecRecord[i].m_mapFileTime.end();++iter)
			{
				string strPath = m_vecRecord[i].m_mapFilePath[iter->first];
				FILETIME ft;
				if(GetFileAmendTime(strPath,&ft))
					iter->second = ft;
			}
		}
	}
}

void CRecordManager::CheckDependenceFun()
{
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (!m_vecRecord[i].m_bOK)
		{
			SetDependeceState(m_vecRecord[i].m_strFunction);
		}
	}
}

void CRecordManager::SetDependeceState(string strFunName)
{
	for (map<string,vector<string> >::iterator iter = m_mapDepedence.begin();iter!=m_mapDepedence.end();++iter)
	{
		if (find(iter->second.begin(),iter->second.end(),strFunName) != iter->second.end())
		{
			int index;
			string strFunName = iter->first;
			FindRecordData(strFunName,index);
			if (m_vecRecord[index].m_bOK)
			{
				m_vecRecord[index].m_bOK = false;
				SetDependeceState(m_vecRecord[index].m_strFunction);
				return;
			}
		}
	}
}

void CRecordManager::SetCommonDependece()
{
	string strCommonFun("CommonDepence");
	for (int i=0;i<(int)m_vecRecord.size();++i)
	{
		if (m_vecRecord[i].m_strFunction == strCommonFun)
		{
			m_vecRecord[i].m_bOK = true;
			SaveCurrentTime(strCommonFun);
		}
	}
}
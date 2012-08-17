#include "stdafx.h"
#include <afxinet.h>
#include <iostream>
#include <sstream>
#include "CHttpUploader.h"

string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value,string::size_type pos = 0)   
{   
	for( pos;   pos!=string::npos;   pos+=new_value.length())  
	{   
		if( (pos=str.find(old_value,pos)) != string::npos )   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}   
	return   str;   
}

bool CHttpUploader::UploadFile(LPCSTR strUrl, LPCSTR strLocalFileName)
{
//	Ast(strUrl != NULL && strLocalFileName != NULL);
	BOOL bResult = FALSE;
	DWORD dwType = 0;
	CString strServer="10.10.43.33";
	CString strObject="file";
	INTERNET_PORT wPort = 80;
	DWORD dwFileLength = 0;
	char* pFileBuf = NULL;
	string filepath = strLocalFileName;
	replace_all_distinct(filepath, "/", "\\");
	string filename = filepath.substr(filepath.find_last_of("\\")+1);
	string headinfo = filename+"=";
	CHttpConnection * pHC = NULL;
	CHttpFile* pHF = NULL;
	CInternetSession cis;
	bResult = AfxParseURL(strUrl, dwType,strServer,strObject,wPort);
	if (!bResult)
		return FALSE;
	CFile file;
	
	try{
		if (!file.Open(strLocalFileName, CFile::shareDenyNone | CFile::modeRead))
			return FALSE;
		dwFileLength = (DWORD)file.GetLength();
		if (dwFileLength <= 0)
			return FALSE;
		dwFileLength;
		pFileBuf = new char[dwFileLength+headinfo.size()];
		memset(pFileBuf,0,sizeof(char)*dwFileLength);
		strncpy(pFileBuf,headinfo.data(),headinfo.size());
		file.Read(pFileBuf+headinfo.size(), dwFileLength);

		const int nTimeOut = 5000;
		cis.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut);
		cis.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 0);
		pHC = cis.GetHttpConnection(strServer, wPort);

		pHF = pHC->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
		if (!pHF->SendRequest(NULL, 0, pFileBuf, dwFileLength+headinfo.size()))
		{
			delete [] pFileBuf;
			pFileBuf = NULL;
			pHF->Close();
			pHC->Close();
			cis.Close();
			return false;
		}
		DWORD dwStateCode = 0;
		pHF->QueryInfoStatusCode(dwStateCode);
		if (dwStateCode = HTTP_STATUS_OK)
			bResult = TRUE;
	}
	catch(CInternetException* pEx)
	{
		char sz[256] = "";
		pEx->GetErrorMessage(sz, 25);
		CString str;
		str.Format("InternetException occur!\r\n%s", sz);
		//AfxMessageBox(str);
	}
	catch(CFileException& fe)
	{
		CString str;
		str.Format("FileException occur!\r\n%d",fe.m_lOsError);
		//AfxMessageBox(str);
	}
	catch(...)
	{
		DWORD dwError = GetLastError();
		CString str;
		str.Format("Unknow Exception occur!\r\n%d", dwError);
		//AfxMessageBox(str);
	}
	delete []pFileBuf;
	pFileBuf = NULL;
	file.Close();
	pHF->Close();
	pHC->Close();
	cis.Close();
	return TRUE;
}

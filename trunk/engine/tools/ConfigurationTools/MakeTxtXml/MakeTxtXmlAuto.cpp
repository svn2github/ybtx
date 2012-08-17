#include "MakeTxtXmlAuto.h"
#include "stdafx.h"
#include "COriFileOperate.h"
#include "tinyxml/tinyxml.h"
#include "CSplitInfo.h"
#include "CExpLog.h"

using namespace std;
#pragma warning(push)
#pragma warning(disable:4018)
void GetColType(string& strFileName,vector<bool>& vecType)
{
	COriFileOperate oriFile(strFileName);
	if (!CSplitInfo::GetInst()->GetBeKeepBlankLine(strFileName))
	{
		oriFile.CleanBlanRow();
		oriFile.CleanNoteRow();
	}
	size_t nRowNum = oriFile.GetRowCount();
	size_t nColNum = 0;
	nColNum = oriFile.GetColCountByRow(0);
	vecType.clear();
	for (size_t i=0;i<nColNum;++i)
	{
		vecType.push_back(true);
	}
	for (size_t i = 1; i < nRowNum; i++)
	{
		for (size_t j = 0; j < nColNum; j++)
		{
			if (!oriFile.BeNumber(i,j))
			{
				vecType[j] = false;
			}
		}
	}
}

void WriteToXml(string& strXml, vector<bool>& vecType,string& FileName)
{
	COriFileOperate oriFile(FileName);
	size_t nVecSize = vecType.size();
	if (strXml.find("chinese_simple") != -1)
	{
		for (size_t i=1;i<nVecSize;++i)
		{
			vecType[i] = false;
		}
	}
	char* szNumOrStrInfo = new char[nVecSize+1];
	for (size_t i = 0; i < nVecSize; i++)
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
	TiXmlDocument* pXmlDoc = new TiXmlDocument;
	pXmlDoc->InsertEndChild(TiXmlElement("config"));
	TiXmlNode* pXmlNode = pXmlDoc->FirstChild("config");
	pXmlNode->InsertEndChild(TiXmlElement("TableType"));
	TiXmlElement* pXmlElem = pXmlNode->FirstChildElement("TableType");
	pXmlElem->InsertEndChild(TiXmlText("S"));
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
void ForceDeleteFile(const string& strFile)
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
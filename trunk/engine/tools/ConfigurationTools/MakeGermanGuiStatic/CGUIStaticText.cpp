#include "stdafx.h"
#include "CGUIStaticText.h"
#include "common.h"
#include "CSplitInfo.h"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

CGUIStaticText::CGUIStaticText()
:m_btoUtf16(true)
,m_bEnablePatch(false)
{
	//m_strChinesePath = "../../cfg/lang/chinese_simple/gui/";
	CSplitInfo* pInst = CSplitInfo::GetInst();
	m_strChinesePath = pInst->GetLangPath() + "/" + pInst->GetOriLang() + "/" + pInst->GetGuiFolder() + "/";

	//m_strDestPath = "germanfolder/cfg/gui/";
	m_strDestPath = pInst->GetTransExportPath() +"/cfg/" + pInst->GetGuiFolder() + "/";

	m_strFile = pInst->GetGuiStaticText();
}

void CGUIStaticText::ReadUtf8Txt(const string& strFileName,vector<vector<string> >& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	string str;
	while(getline(inFile,str))
	{
		if(str.empty())
			continue;
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

void CGUIStaticText::ReadUtf16Txt(const string& strFileName,vector<vector<wstring> >& vecRef)
{
	ifstream inFile(strFileName.c_str(),ios_base::binary | ios_base::in);
	wchar_t wcharTemp;
	wstring wstr;
	vector<vector<wstring> > vecRead;
	vecRead.push_back(vector<wstring>());
	inFile.seekg(2,ios::beg);
	while (inFile.peek() != EOF)
	{
		inFile.read((char*)(&wcharTemp),2);
		if (wcharTemp == 0x0009)
		{
			vecRead.back().push_back(wstr);
			wstr = L"";
		}
		else if (wcharTemp == 0x000D)
		{
			vecRead.back().push_back(wstr);
			wstr = L"";
			inFile.seekg(2,ios::cur);
			vecRead.push_back(vector<wstring>());
		}
		else
		{
			wstr.append(1,wcharTemp);
		}
	}
	inFile.close();
}

void CGUIStaticText::WriteUtf8ToUtf16(const string& strFileName,vector<vector<string> >& vecRef)
{
	ofstream outFile(strFileName.c_str(),ios_base::out | ios_base::binary);
	if (!outFile)
		return;
	char headArray[2];
	headArray[0] = uint8(15*16+15);
	headArray[1] = uint8(15*16+14);
	outFile.write(headArray,2);
	char enterArray[4];
	enterArray[0] = 13;
	enterArray[1] = 0;
	enterArray[2] = 10;
	enterArray[3] = 0;
	char tableArray[2];
	tableArray[0] = 9;
	tableArray[1] = 0;
	wstring wstrTemp;
	for (int i=0;i<vecRef.size();++i)
	{
		for (int j=0;j<vecRef[0].size();++j)
		{
			wstrTemp = Utf8ToUCS2(vecRef[i][j]);
			outFile.write((char*)wstrTemp.c_str(),wstrTemp.size()*2);
			if (j<vecRef[0].size()-1)
			{
				outFile.write(tableArray,2);
			}
			else
			{
				outFile.write(enterArray,4);
			}
		}
	}
	outFile.close();
}

void CGUIStaticText::WriteUtf16ToUtf8(const string& strFileName,vector<vector<wstring> >& vecRef)
{
	ofstream outFile(strFileName.c_str());
	if (!outFile)
		return;
	for (int i=0;i<vecRef.size();++i)
	{
		for (int j=0;j<vecRef[i].size();++j)
		{
			string strTemp = utf16_to_utf8(vecRef[i][j]);
			outFile<<strTemp;
			if (j<vecRef[i].size()-1)
				outFile<<"\t";
		}
		outFile<<"\n";
	}
	outFile.close();
}
void CGUIStaticText::ConverseToGermanLangSrc()
{
	string strName = m_strChinesePath + m_strFile;
	string strSaveName = m_strDestPath + m_strFile;
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strName,vecTemp);
	string strPath = strSaveName;
	int pos = -1;
	pos = strPath.rfind('/');
	strPath = strPath.substr(0,pos);
	MakeDirectory( gbk_to_utf16(strPath).c_str() );
	strSaveName = GetPatchName(strSaveName);
	WriteUtf8ToUtf16(strSaveName,vecTemp);
}

void CGUIStaticText::ConverseToGermanLangTgt()
{
	string strName = m_strDestPath + m_strFile;
	string strSaveName = m_strChinesePath;
	const string& strChineseFolder = CSplitInfo::GetInst()->GetOriLang();
	const string& strForeignFolder = CSplitInfo::GetInst()->GetForeLang();
	replace_all_distinct(strSaveName, strChineseFolder, strForeignFolder, 0);
	vector<vector<wstring> > vecTemp;
	ReadUtf16Txt(strName,vecTemp);
	WriteUtf16ToUtf8(strSaveName,vecTemp);
}

string CGUIStaticText::GetPatchName(string strName)
{
	if (m_bEnablePatch)
	{
		string strResult = strName;
		strResult = strResult + ".temp";
		return strResult;
	}
	return strName;
}


#pragma warning(pop)

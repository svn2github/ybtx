#include "stdafx.h"
#include "CSvsFile.h"
#include "ExpHelper.h"
#include "StringHelper.h"
#include "tinyxml/tinyxml.h"
#include "CodeCvs.h"

string sStrHeadInfo = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<body xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" mimetype=\"vnd.svs.spreadsheet\">\n<table name=\"EXCEL\" border=\"1\">\n";
string sStrEndInfo = "</table>\n</body>\n";

string sFileXMLVer = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

void CSvsFile::LoadXML()
{
	UnloadXML();
	TiXmlDocument XmlDoc;
	XmlDoc.SetCondenseWhiteSpace(false);
	ifstream iXML( m_strFile.c_str() );
	if (!iXML)
	{
		GenErr("技能配置表转换文件读取错误", m_strFile);
	}
	iXML>>XmlDoc;
	iXML.close();

	TiXmlNode* pBody = XmlDoc.FirstChild("body");
	TiXmlNode* pTable = pBody->FirstChild("table");

	for ( TiXmlNode* pTr = pTable->FirstChild(); pTr; pTr = pTr->NextSibling() )
	{
		TiXmlElement* pElemet = pTr->ToElement();
		if (!pElemet)
			continue;

		if(pElemet->ValueStr() != "tr")
			continue;

		m_vecTR.resize(m_vecTR.size() + 1);
		TableRow& vecTD = m_vecTR.back();		
		for ( TiXmlNode* pTd = pTr->FirstChild(); pTd; pTd=pTd->NextSibling() )
		{
			TiXmlElement* pElemet = pTd->ToElement();
			if (!pElemet)
				continue;

			if(pElemet->ValueStr() != "td")
				continue;

			TiXmlNode* pParagraph = pTd->FirstChild();
			TiXmlElement* pParaElemet = pParagraph->ToElement();
			if(!pParaElemet || pParaElemet->ValueStr() != "p")
			{
				GenErr("<td>读取<p>不存在");
			}

			TableData aTD;

			aTD.m_eTDType = GetTableDataType(pParaElemet->Attribute("t"));
			if(pParaElemet->FirstChild())
			{
				aTD.m_sTDValue = utf8_to_gbk(pParaElemet->GetText());
				XMLDecode(aTD.m_sTDValue);
			}

			vecTD.push_back(aTD);
		}
	}
}

void CSvsFile::SaveXML()
{
	ofstream outStream(m_strFile.c_str());
	if (!outStream)
	{
		string errorStr = m_strFile;
		errorStr = errorStr + "打开时候发生错误，请检查是否有其他程序正在使用中！";
		GenErr(errorStr);
	}
	outStream << sStrHeadInfo;
	string strTopLeft = m_vecTR[0][0].m_sTDValue;
	trim(strTopLeft);
	if(strTopLeft.size() >= 5)
	{
		strTopLeft = strTopLeft.substr(0, 5);
		//transform(strTopLeft.begin(), strTopLeft.end(), strTopLeft.begin(), toupper);	//这句不知为何运行报错
		//if(strcmp(strupr(const_cast<char*>(strTopLeft.c_str())), "NOTE:") == 0 && m_vecTR.size() > 1)
		if(strTopLeft == "NOTE:" && m_vecTR.size() > 1)
			m_uTitleLineNo = 1;
	}
	vector<string> m_sColName;
	//printf("m_uTitleLineNo = %d, m_vecTR[m_uTitleLineNo].size() = %d\n", m_uTitleLineNo, m_vecTR[m_uTitleLineNo].size());
	//m_sColName.resize(m_vecTR[m_uTitleLineNo].size());		//这句不知为何在Release下会报错
	for(size_t col = 0; col < m_vecTR[m_uTitleLineNo].size(); ++col)
	{
		m_sColName.push_back(m_vecTR[m_uTitleLineNo][col].m_sTDValue);
		XMLEncode(m_sColName.back(), true);
		m_sColName.back() = gbk_to_utf8(m_sColName.back());
	}
	for(size_t row = 0; row < m_vecTR.size(); ++row)
	{
		outStream << "<tr>\n";
		for(size_t col = 0; col < m_vecTR[row].size(); ++col)
		{
			outStream << "<td n=\"" << m_sColName[col] << "\">\n<p t=\"";
			TableData& aTD = m_vecTR[row][col];
			switch(aTD.m_eTDType)
			{
			case eTDT_None:
				outStream << "\"/>";
				break;
			case eTDT_String:
				XMLEncode(aTD.m_sTDValue);
				outStream << "string\">" << gbk_to_utf8(aTD.m_sTDValue) << "</p>";
				break;
			case eTDT_Float:
				outStream << "float\">" << aTD.m_sTDValue << "</p>";
				break;
			default:
				{
					stringstream str;
					str << aTD.m_eTDType;
					GenErr("CSvsFile::SaveXML() aTD.m_eTDType类型错误", str.str());
				}
			}
			outStream << "</td>\n";
		}
		outStream << "</tr>\n";
	}
	outStream << sStrEndInfo;
	outStream.close();
}

CSvsFile::ETableDataType CSvsFile::GetTableDataType(const string& sType)
{
	if(sType == "string")
	{
		return eTDT_String;
	}
	else if(sType == "float")
	{
		return eTDT_Float;
	}
	else if(sType == "")
	{
		return eTDT_None;
	}
	else
	{
		GenErr("GetTableDataType类型超出范围", sType);
		return eTDT_None;
	}
}

void CSvsFile::UnloadXML()
{
	//if(m_pXmlDoc)
	//{
	//	m_pXmlDoc->Clear();
	//	delete m_pXmlDoc;
	//	m_pXmlDoc = NULL;
	//}

	m_vecTR.clear();
}

CSvsFile::CSvsFile(string& strFileName)
: m_strFile(strFileName)
, m_uTitleLineNo(0)
{
	//m_strFile = "../../cfg/data/src./skill/designer_GiftSkill_Common1s.svs";
	LoadFile();
}

CSvsFile::CSvsFile()
: m_uTitleLineNo(0)
{

}

CSvsFile::~CSvsFile()
{
	UnloadXML();
}

string CSvsFile::GetDataByRowCol(size_t row,size_t col)
{
	CheckRowCol(row,col);
	return m_vecTR[row][col].m_sTDValue;
}

bool CSvsFile::BeNumber(size_t row,size_t col)
{
	CheckRowCol(row,col);
	return m_vecTR[row][col].m_eTDType == eTDT_Float;
}

void CSvsFile::AddData_toTab(string& strContent,size_t row,size_t col,bool bNum)
{
	AddRowColPos(row,col);
	TableData& aTD = m_vecTR[row][col];
	if(bNum)
		aTD.m_eTDType = eTDT_Float;
	else if(!strContent.empty())
		aTD.m_eTDType = eTDT_String;
	else
		aTD.m_eTDType = eTDT_None;
	aTD.m_sTDValue = strContent;
}

void CSvsFile::InsertData_toTab(string& strContent,size_t row,size_t col,bool bNum)
{
	if (m_vecTR.empty() || row >= m_vecTR.size())
		return;
	TableData aTD;
	if(bNum)
		aTD.m_eTDType = eTDT_Float;
	else if(!strContent.empty())
		aTD.m_eTDType = eTDT_String;
	else
		aTD.m_eTDType = eTDT_None;
	aTD.m_sTDValue = strContent;
	m_vecTR[row].insert(m_vecTR[row].begin() + col, aTD);
}

void CSvsFile::AddRowColPos(size_t row,size_t col)
{
	if(row >= m_vecTR.size())
	{
		m_vecTR.resize(row + 1);
	}

	if(col >= m_vecTR[row].size())
	{
		m_vecTR[row].resize(col + 1);
	}
}

void CSvsFile::CheckRowCol(size_t row,size_t col)
{
	if(row >= m_vecTR.size() || m_vecTR[row].empty() || col >= m_vecTR[row].size())
	{
		stringstream str;
		str << m_strFile << ": row = " << row << "col = " << col;
		GenErr("CSvsFile::CheckRowCol参数超出范围", str.str());
	}
}

//void CSvsFile::GetFileContent(vector<vector<string>>& vecRef)
//{
//	vecRef.clear();
//	vecRef.resize(m_vecTR.size() + 1);
//	for(size_t i = 0; i < m_vecTR.size(); ++i)
//	{
//		TableRow& vecTD = m_vecTR[i];
//		for(size_t j = 0; j < vecTD.size(); ++j)
//		{
//			TableData& aTD = vecTD[j];
//			vecRef[i].push_back(aTD.m_sTDValue);
//		}
//	}
//}

void CSvsFile::LoadFile()
{
	LoadXML();
}

size_t CSvsFile::GetRowCount()
{
	return m_vecTR.size();
}

size_t CSvsFile::GetColCountByRow(size_t row)
{
	if(m_vecTR.size() <= row)
	{
		stringstream str;
		str << m_strFile << ": row = " << row << ", m_vecTR.size() = " << m_vecTR.size();
		GenErr("CSvsFile::GetColCountByRow参数row >= m_vecTR.size()", str.str());
	}
	return m_vecTR[row].size();
}

void CSvsFile::SaveFile()
{
	SaveXML();
}


void CSvsFile::DeleteData(size_t row,size_t col)
{
	CheckRowCol(row, col);
	m_vecTR[row][col] = TableData();
}

void CSvsFile::DeleteRow(size_t row)
{
	if(row < m_vecTR.size())
	{
		m_vecTR.erase(m_vecTR.begin() + row);
	}
}

void CSvsFile::DeleteCol(size_t col)
{
	if(m_vecTR.empty() || col >= m_vecTR[m_uTitleLineNo].size())
		return;
	for(size_t i = m_vecTR.size(); i > 0; --i)
	{
		m_vecTR[i - 1].erase(m_vecTR[i - 1].begin() + col);
	}
}

void CSvsFile::DeleteColByRow(size_t row,size_t col)
{
	CheckRowCol(row,col);
	m_vecTR[row].erase(m_vecTR[row].begin() + col);
}

void CSvsFile::AddRow_Back()
{
	m_vecTR.resize(m_vecTR.size() + 1);
}

void CSvsFile::Resize(size_t row, size_t col)
{
	m_vecTR.resize(row);
	for(size_t i = 0; i < row; ++i)
	{
		m_vecTR[i].resize(col);
	}
}

void CSvsFile::XMLEncode(string& sText, bool bIsAttr)
{
	replace(sText, "<", "&lt;");
	replace(sText, ">", "&gt;");
	replace(sText, "&", "&amp;");
	if(bIsAttr)
		replace(sText, "\"", "&#34;");
}

void CSvsFile::XMLDecode(string& sText, bool bIsAttr)
{
	replace(sText, "&lt;", "<");
	replace(sText, "&gt;", ">");
}

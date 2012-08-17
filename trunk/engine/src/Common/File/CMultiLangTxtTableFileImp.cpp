#include "stdafx.h"
#include "CMultiLangTxtTableFile.h"
#include "CMultiLangTxtTableFileImp.h"
#include "CCurLangTxtTableFileImp.h"

#include "PkgStream.h"
#include "CXmlConfig.inl"
#include "TSqrAllocator.inl"
#include "TBaseTableFileImp.inl"

template class TBaseTableFileImp<CMultiLangTxtTableFileImp>;

CMultiLangTxtTableFileImp::CMultiLangTxtTableFileImp()
: Parent_t(true)
, m_bUseMultiLang(false)
, m_pCurLangImp(NULL)
, m_bRedirectOffsetFinished(false)
{

}

CMultiLangTxtTableFileImp::~CMultiLangTxtTableFileImp()
{

}

bool CMultiLangTxtTableFileImp::GetUseMultiLang()
{
	return m_bUseMultiLang;
}

void CMultiLangTxtTableFileImp::SetUseMultiLang(bool bUseMultiLang)
{
	m_bUseMultiLang = bUseMultiLang;
}

void CMultiLangTxtTableFileImp::RedirectOffset(CCurLangTxtTableFileImp* pCurLangImp)
{
	Lock();
	if(m_bRedirectOffsetFinished)
	{
		UnLock();
		return;
	}
	m_bRedirectOffsetFinished = true;
	if(!pCurLangImp)
	{
		GenLoadError("", "CMultiLangTxtTableFileImp::RedirectOffset()的参数不能为空", "");
	}

	m_pCurLangImp = pCurLangImp;

	m_vecOffsetToCurLang.assign(m_TableData.vecOffset.size(), false);

	vector<pair<int32, int32> > vecColOriMapToCur;
	
	int32 iSrcWidth = m_TableData.nWidth; 
	for(int32 iSrcCol = 0; iSrcCol < iSrcWidth; ++iSrcCol)
	{
		char* szKeyName = &m_TableData.vecBuf[m_TableData.vecOffset[iSrcCol]];
		int32 iTgtCol = m_pCurLangImp->GetColNo(szKeyName);
		if(iTgtCol != -1)
			vecColOriMapToCur.push_back(make_pair(iSrcCol, iTgtCol));
	}

	int32 iColRedirectSize = (int32)vecColOriMapToCur.size();
	int32 iSrcHeight = m_TableData.nHeight;
	int32 iTgtWidth = m_pCurLangImp->GetWidth();
	for(int32 iSrcRow = 1; iSrcRow < iSrcHeight; ++iSrcRow)
	{
		const char* szKeyName = &m_TableData.vecBuf[m_TableData.vecOffset[iSrcWidth * iSrcRow]];
		int32 iTgtRow = pCurLangImp->GetRowNo(szKeyName);
		if(iTgtRow != -1)
		{
			for(int32 iRedir = 1; iRedir < iColRedirectSize; ++iRedir)
			{
				int32 iSrcCol = vecColOriMapToCur[iRedir].first;
				int32 iTgtCol = vecColOriMapToCur[iRedir].second;
				int32 iSrcPos = iSrcRow * iSrcWidth + iSrcCol;
				int32 iTgtPos = iTgtRow * iTgtWidth + iTgtCol;
				m_vecOffsetToCurLang[iSrcPos] = true;
				m_TableData.vecOffset[iSrcPos] = iTgtPos;
			}
		}
	}
	UnLock();
}

const char* CMultiLangTxtTableFileImp::GetString(int32 nRow, int32 nCol)
{
	if(!GetUseMultiLang())
		return Parent_t::GetString(nRow, nCol);

	if (nRow < m_TableData.nHeight && nCol < m_TableData.nWidth)
	{
		int32 nOffsetIndex = nRow * m_TableData.nWidth + nCol;
		if(m_vecOffsetToCurLang[nOffsetIndex])
		{
			return m_pCurLangImp->GetString(m_TableData.vecOffset[nOffsetIndex]);
		}
		else
		{
			return Parent_t::GetString(nOffsetIndex);
		}
	}

	return "";
}

const char* CMultiLangTxtTableFileImp::GetString(int32 nRow, const char* szColName)
{
	if(!GetUseMultiLang())
		return Parent_t::GetString(nRow, szColName);

	int32 nCol = GetCol(szColName);
	if (nCol == -1)
	{
		return "";
	}

	if (nRow < m_TableData.nHeight)
	{
		int32 nOffsetIndex = nRow * m_TableData.nWidth + nCol;
		if(m_vecOffsetToCurLang[nOffsetIndex])
		{
			return m_pCurLangImp->GetString(m_TableData.vecOffset[nOffsetIndex]);
		}
		else
		{
			return Parent_t::GetString(nOffsetIndex);
		}
	}

	return "";
}

const char* CMultiLangTxtTableFileImp::GetString(int32 nOffsetIndex)
{
	if(!GetUseMultiLang())
		return Parent_t::GetString(nOffsetIndex);

	if(nOffsetIndex >= 0 && nOffsetIndex < (int32)m_TableData.vecOffset.size())
	{
		if(m_vecOffsetToCurLang[nOffsetIndex])
		{
			return m_pCurLangImp->GetString(m_TableData.vecOffset[nOffsetIndex]);
		}
		else
		{
			return Parent_t::GetString(nOffsetIndex);
		}
	}

	return "";
}



ETableFileFieldType CMultiLangTxtTableFileImp::GetType(int32 nCol)
{
	if (nCol >= (int32)m_vecType.size() || nCol < 0)
	{
		return eTFFT_Empty;
	}
	return m_vecType[nCol];
}

ETableFileFieldType CMultiLangTxtTableFileImp::GetType(string szColName)
{
	MapName2Type::iterator iter = m_mName2Type.find(szColName);
	if (iter != m_mName2Type.end())
	{
		return iter->second;
	}
	return eTFFT_Empty;

}
void CMultiLangTxtTableFileImp::Map2Vec()
{
	for(uint32 i=0;i<m_mName2Type.size();i++)
	{
		string str = GetNamefromCol(i);
		MapName2Type::iterator iter = m_mName2Type.find(str);
		if (iter != m_mName2Type.end())
		{
			m_vecType[i]=iter->second;
		}
		else 
		{	
			ostringstream strMsg ;
			strMsg<<"txt中的列名为"<<str<<"；XML中的全部列名为：";
			for(iter = m_mName2Type.begin();iter!=m_mName2Type.end();iter++)
			{
				strMsg<<iter->first<<"、";
			}
			GenLoadError(NULL,"单行表xml文件中的列名和txt中的列名不一致",strMsg.str().c_str());
			m_vecType[i]=eTFFT_Empty;
		}
	}
}
bool CMultiLangTxtTableFileImp::LoadXMLFileDerived(const wchar_t* szAliasPath, const char* szFileName)
{
	//载入xml
	ipkgstream ifXml(szAliasPath, szFileName);
	if (!ifXml)
	{
		GenLoadError(szFileName, "TxtTable没有对应的xml文件");
		return false;
	}
	CXmlConfig XmlCfg;
	XmlCfg.LoadConfig("config", ifXml);
	ifXml.close();
	
	TiXmlNode* pNode = XmlCfg.GetRootNode();
	Ast(pNode);
	const char* szType = pNode->FirstChild("TableType")->ToElement()->GetText();
	char cType = szType[0];
	if (cType != 'S' && cType != 's')
	{
		GenLoadError(szFileName, "XML文件类型不正确，TableType不是s或者S");
		return false;
	}
	for(TiXmlNode* pFileName = pNode->FirstChild("Col");pFileName;pFileName = pFileName->NextSibling("Col"))
	{
		Ast(pFileName);
		string szName1 = pFileName->ToElement()->Attribute("Name");
		string szType1 = pFileName->ToElement()->Attribute("Type");
		if(szType1=="s"||szType1=="S")
		{
			m_mName2Type.insert(make_pair(szName1,eTFFT_String));
		}
		else if(szType1=="n"||szType1=="N")
		{
			m_mName2Type.insert(make_pair(szName1,eTFFT_Number));
		}
		else
		{
			GenLoadError(szFileName, "XML数据类型不正确，类型只能为s或者n", szType1);
			return false;
		}
	}
	return true;
}

bool CMultiLangTxtTableFileImp::LoadDerived()
{
	m_vecType.resize(m_mName2Type.size());
	Map2Vec();
	if (GetWidth() != (int32)m_vecType.size())
	{
		ostringstream strm;
		strm << "XML配置文件里面设定的宽度为: " << m_vecType.size();
		strm << "Txt数据文件实际的宽度为: " << GetWidth();
		GenLoadError(NULL, "TableFile格式不正确", strm.str().c_str());
		return false;
	}

	return true;
}


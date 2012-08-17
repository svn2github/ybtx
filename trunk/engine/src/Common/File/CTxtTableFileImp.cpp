#include "stdafx.h"
#include "CTxtTableFileImp.h"
#include "PkgStream.h"
#include "CXmlConfig.inl"
#include "CTxtTableFile.h"
#include "TSqrAllocator.inl"
#include "TBaseTableFileImp.inl"

template class TBaseTableFileImp<CTxtTableFileImp>;

CTxtTableFileImp::CTxtTableFileImp()
: Parent_t(true)
{
}

CTxtTableFileImp::~CTxtTableFileImp()
{
}


ETableFileFieldType CTxtTableFileImp::GetType(int32 nCol)
{	
	if (nCol >= (int32)m_vecType.size() || nCol < 0)
	{
		return eTFFT_Empty;
	}
	return m_vecType[nCol];
}

ETableFileFieldType CTxtTableFileImp::GetType(string szColName)
{
	MapName2Type::iterator iter = m_mName2Type.find(szColName);
	if (iter != m_mName2Type.end())
	{
		return iter->second;
	}
	return eTFFT_Empty;
}
void CTxtTableFileImp::Map2Vec()
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
bool CTxtTableFileImp::LoadXMLFileDerived(const wchar_t* szAliasPath, const char* szFileName)
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

bool CTxtTableFileImp::LoadDerived()
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


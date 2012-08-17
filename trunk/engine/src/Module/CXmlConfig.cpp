#include "stdafx.h"
#include "CXmlConfig.h"
#include "tinyxml/tinyxml.h"
#include "ExpHelper.h"
#include "BaseHelper.h"

CXmlSiblingElemIter::CXmlSiblingElemIter()
{

}

void CXmlSiblingElemIter::SetCurNode(TiXmlNode* pNode, const char* szElemName)
{
	m_pCurNode = pNode;
	m_strSiblingName = szElemName;
}

void CXmlSiblingElemIter::MoveNext()
{
	if (m_pCurNode)
	{
		TiXmlNode* pNode = m_pCurNode;
		while ((pNode = pNode->NextSiblingElement()))
		{
			if (pNode->Value() == m_strSiblingName)
			{
				break;
			}
		}
		m_pCurNode = pNode;
	}
}

bool CXmlSiblingElemIter::IsEnd()
{
	return m_pCurNode == NULL;
}


const char* CXmlSiblingElemIter::Get(const char* szElemName)const
{
	const char* szText=NULL;

	if (m_pCurNode)
	{
		TiXmlNode* pNode = m_pCurNode->FirstChildElement( szElemName );
		if (pNode)
		{
			szText = pNode->ToElement()->GetText();
		}
	}
	return szText;
}

const char* CXmlSiblingElemIter::GetSiblingName()const
{
	return m_strSiblingName.c_str();
}

CXmlConfig::CXmlConfig()
{
	//因为要鼓励大家使用与文件名相同的根节点名字，因此这里产生随机的根节点名称
	//这样的设计，使得XmlConfig不会存在“无效”的中间态，同时也不易使用，起到鼓励使用带有节点名称的构造函数的作用
	char szName[32];
	sprintf( szName, "XmlConfig_%d" ,rand() );

	m_pRoot	= new TiXmlElement(szName);
}

CXmlConfig::CXmlConfig(const char* szRootNodeName)
{
	m_pRoot	= new TiXmlElement(szRootNodeName);
}

CXmlConfig::CXmlConfig(const char* szRootNodeName,istream& Stream )
{
	m_pRoot = NULL;
	LoadConfig( szRootNodeName,Stream );
}

CXmlConfig::CXmlConfig(TiXmlElement* pElement)
{
	m_pRoot=static_cast<TiXmlElement*>( pElement->Clone() );
}

CXmlConfig::~CXmlConfig()
{
	Clear();
}

void CXmlConfig::Clear()
{
	delete m_pRoot;
	m_pRoot=NULL;
}

bool CXmlConfig::LoadConfig(const char* szRootNodeName,const char* szFileName)
{
	if( !szRootNodeName )
		GenErr("配置文件的根节点名称不能为NULL");


	TiXmlDocument XmlDoc;

	if( !XmlDoc.LoadFile( szFileName ) )
		return false;

	TiXmlElement* pRoot = XmlDoc.FirstChildElement( szRootNodeName );

	if( !pRoot )
	{
		ostringstream strm;
		strm<<"配置文件的根节点不是\""<<szRootNodeName<<"\"";
		GenErr( strm.str() );
	}	
	Clear();

	m_pRoot = static_cast<TiXmlElement*>( pRoot->Clone() );

	return true;
}

void CXmlConfig::LoadConfig(const char* szRootNodeName,istream& Stream)
{	
	if( !szRootNodeName )
		GenErr("配置文件的根节点名称不能为NULL");


	TiXmlDocument XmlDoc;

	Stream>>XmlDoc;

	TiXmlElement* pRoot = XmlDoc.FirstChildElement( szRootNodeName );

	if( !pRoot )
	{
		ostringstream strm;
		strm<<"配置文件的根节点不是\""<<szRootNodeName<<"\"";
		GenErr( strm.str() );
	}	
	Clear();

	m_pRoot = static_cast<TiXmlElement*>( pRoot->Clone() );
}

bool CXmlConfig::LoadConfigFormString(const char* szRootNodeName, const char* szXml)
{
	if( !szRootNodeName )
	{
		GenErr("配置文件的根节点名称不能为NULL");
		return false;
	}
	TiXmlDocument XmlDoc;

	XmlDoc.Parse(szXml);

	TiXmlElement* pRoot = XmlDoc.FirstChildElement( szRootNodeName );

	if( !pRoot )
	{
		ostringstream strm;
		strm<<"配置文件的根节点不是\""<<szRootNodeName<<"\"";
		GenErr( strm.str() );
		Clear();
		return false;
	}	
	Clear();

	m_pRoot = static_cast<TiXmlElement*>( pRoot->Clone() );
	return true;
}

void CXmlConfig::SaveConfig(const char* szFileName)
{
	TiXmlElement* tmpRoot=static_cast<TiXmlElement*>( m_pRoot->Clone() );

	TiXmlDocument xmlDoc;
	xmlDoc.LinkEndChild( tmpRoot );
	xmlDoc.SaveFile( szFileName );
}


TiXmlElement* CXmlConfig::GetRootNode()
{
	return m_pRoot;
}

TiXmlElement* CXmlConfig::GetNode(const vector<const char*>& vecElemName,bool bGenErr)const
{
	TiXmlElement* pNode = m_pRoot;

	for( size_t i=0 ; i<vecElemName.size(); ++i )
	{
		const char* szElemName = vecElemName[i];

		pNode = pNode->FirstChildElement( szElemName );

		if( pNode )
			continue;

		if( !bGenErr )
			return NULL;

		ostringstream strm;

		strm<<"Can't find config element \""<< m_pRoot->Value() << "/";

		for( size_t j=0 ; j<=i; ++j )
			strm << vecElemName[j] << "/";

		strm<<"\"";

		GenErr( strm.str() );
	}

	return pNode;
}

void  CXmlConfig::GetIter(CXmlSiblingElemIter* it, const char* szElemName)const
{
	//CXmlSiblingElemIter it;
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName);
	it->SetCurNode(GetNode( vecElemName, false ), szElemName);
	//return it;
}

void CXmlConfig::GetIter(CXmlSiblingElemIter* it, const char* szElemName1,const char* szElemName2)const
{
	//CXmlSiblingElemIter it;
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );
	it->SetCurNode(GetNode( vecElemName, false ), szElemName2);
	//return it;
}

void CXmlConfig::Set(const char* szParElemName,const char* szChildElemName, const char* szElemTexName)
{
	vector<const char*> vecParElemNames;
	vecParElemNames.push_back( szParElemName );
	TiXmlNode *pParentNode = this->GetNode(vecParElemNames, false);

	TiXmlElement *pSetElement  = new TiXmlElement(szChildElemName);
	TiXmlText	 *pElementText = new TiXmlText(szElemTexName);
	pSetElement->LinkEndChild(pElementText);
	pParentNode->LinkEndChild(pSetElement);
}

void CXmlConfig::Insert(const char* szParElemName,const char* szChildElemName)
{
	vector<const char*> vecParElemNames;
	vecParElemNames.push_back( szParElemName );
	TiXmlNode *pParentNode = this->GetNode(vecParElemNames, false);
	if( pParentNode == NULL )
		pParentNode = m_pRoot;

	TiXmlElement *pSetElement  = new TiXmlElement(szChildElemName);
	pParentNode->LinkEndChild(pSetElement);
}

void CXmlConfig::AttribSet(const char* szParElemName,const char* szChildElemName)
{
	vector<const char*> vecParElemNames;
	vecParElemNames.push_back( szParElemName );
	TiXmlNode *pParentNode = this->GetNode(vecParElemNames, false);
	if( pParentNode == NULL )
		pParentNode = m_pRoot;

	TiXmlElement *pSetElement = new TiXmlElement(szChildElemName);
	pParentNode->LinkEndChild(pSetElement);
}

void CXmlConfig::AttribSet(const char* szParElemName,const char* szChildElemName, const char* szAttribValueName, const char* szAttribValueValue)
{
	vector<const char*> vecParElemNames;
	vecParElemNames.push_back( szParElemName );
	TiXmlNode *pParentNode = this->GetNode(vecParElemNames, false);
	if( pParentNode == NULL )
		pParentNode = m_pRoot;

	TiXmlElement *pSetElement = new TiXmlElement(szChildElemName);
	pParentNode->LinkEndChild(pSetElement);
	pSetElement->SetAttribute(szAttribValueName, szAttribValueValue);
}

void CXmlConfig::AttribSet(const char* szParElemName,const char* szChildElemName, const vector<string>& vecAttribValueName, const vector<string>& vecAttribValueValue)
{
	vector<const char*> vecParElemNames;
	vecParElemNames.push_back( szParElemName );
	TiXmlNode *pParentNode = this->GetNode(vecParElemNames, false);
	if( pParentNode == NULL )
		pParentNode = m_pRoot;

	TiXmlElement *pSetElement = new TiXmlElement(szChildElemName);
	pParentNode->LinkEndChild(pSetElement);

	size_t size = vecAttribValueName.size();
	for (size_t i = 0; i < size; ++i )
	{
		string name  = vecAttribValueName[i];
		string value = vecAttribValueValue[i];
		pSetElement->SetAttribute(name.c_str(), value.c_str());
	}
}

void CXmlConfig::AttribSet(const char* szParElemName1,const char* szParElemName2, size_t parElemName2Index, const char* szChildElemName, const vector<string>& vecAttribValueName, const vector<string>& vecAttribValueValue)
{
	vector<const char*> vecParElemNames;
	vecParElemNames.push_back( szParElemName1 );
	vecParElemNames.push_back( szParElemName2 );
	TiXmlNode *pParentNode = this->GetNode(vecParElemNames, false);
	if( pParentNode == NULL )
		pParentNode = m_pRoot;

	for ( size_t i = 0; i< parElemName2Index; ++i )
		pParentNode = pParentNode->NextSibling(szParElemName2);

	TiXmlElement *pSetElement = new TiXmlElement(szChildElemName);
	pParentNode->LinkEndChild(pSetElement);

	size_t size = vecAttribValueName.size();
	for (size_t i = 0; i < size; ++i )
	{
		string name  = vecAttribValueName[i];
		string value = vecAttribValueValue[i];
		pSetElement->SetAttribute(name.c_str(), value.c_str());
	}
}

void CXmlConfig::AttribSet(const vector<const char*>& szParElemNames, const char* szChildElemName, const vector<string>& vecAttribValueName, const vector<string>& vecAttribValueValue)
{
	TiXmlNode *pParentNode = this->GetNode(szParElemNames, false);
	if( pParentNode == NULL )
		pParentNode = m_pRoot;

	TiXmlElement *pSetElement = new TiXmlElement(szChildElemName);
	pParentNode->LinkEndChild(pSetElement);

	size_t size = vecAttribValueName.size();
	for (size_t i = 0; i < size; ++i )
	{
		string name  = vecAttribValueName[i];
		string value = vecAttribValueValue[i];
		pSetElement->SetAttribute(name.c_str(), value.c_str());
	}
}

void CXmlConfig::GenElemNoContentErr(const vector<const char*>& vecElemName)const
{
	ostringstream strm;

	strm<<"Config element \""<< m_pRoot->Value() << "/";

	for( size_t j=0 ; j<vecElemName.size(); ++j )
		strm << vecElemName[j] << "/";

	strm<<"\" has no content.";

	GenErr( strm.str() );
}


template<>
const char* CXmlConfig::GetNodeValue<const char*>
(const char*const* pDefaultValue,TiXmlNode* pNode,const vector<const char*>& vecElemName)const
{
	const char* szText=NULL;

	if( !pNode || !(szText=pNode->ToElement()->GetText()) )
	{
		if(pDefaultValue)
			return *pDefaultValue;

		GenElemNoContentErr(vecElemName);
	}

	return szText;
}


void CXmlConfig::GenAttribNoContentErr(const char* szAttribName,const vector<const char*>& vecElemName)const
{
	ostringstream strm;

	strm<<"Attribute \""<<szAttribName<<"\" of config element \""<< m_pRoot->Value() << "/";

	for( size_t j=0 ; j<=vecElemName.size(); ++j )
		strm << vecElemName[j] << "/";

	strm<<"\" has no content.";

	GenErr( strm.str() );
}



template<>
const char* CXmlConfig::GetNodeAttribValue<const char*>
(const char*const* pDefaultValue,TiXmlNode* pNode,const char* szAttribName,const vector<const char*>& vecElemName)const
{
	const char* szText=NULL;

	if( !pNode || !(szText=pNode->ToElement()->Attribute(szAttribName)) )
	{
		if( pDefaultValue )
			return *pDefaultValue;

		GenAttribNoContentErr( szAttribName, vecElemName );
	}

	return szText;
}

template<>
TiXmlElement* CXmlConfig::Get<TiXmlElement*>(const char* szElemName)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName );

	return GetNode( vecElemName, false );
}

template<>
TiXmlElement* CXmlConfig::Get<TiXmlElement*>(const char* szElemName1,const char* szElemName2)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );

	return GetNode( vecElemName, false );
}

template<>
TiXmlElement* CXmlConfig::Get<TiXmlElement*>(const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	vector<const char*> vecElemName;
	vecElemName.push_back( szElemName1 );
	vecElemName.push_back( szElemName2 );
	vecElemName.push_back( szElemName3 );

	return GetNode( vecElemName, false );
}


template<typename NumberType>
NumberType CStrToValue(const char* szText);


template<>
MODULE_API bool CStrToValue<bool>(const char* szText)
{
	return atoi( szText )!=0;
}


template<>
MODULE_API int64 CStrToValue<int64>(const char* szText)
{
#ifdef _WIN32
	return _atoi64( szText );
#else
	return atoll( szText );
#endif
}

template<>
MODULE_API uint64 CStrToValue<uint64>(const char* szText)
{
	return uint64( CStrToValue<int64>(szText) );
}

template<>
MODULE_API float CStrToValue<float>(const char* szText)
{
	return float( atof( szText ) );
}

template<>
MODULE_API double CStrToValue<double>(const char* szText)
{
	return atof( szText );
}


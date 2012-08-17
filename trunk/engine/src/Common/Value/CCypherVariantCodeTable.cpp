#include "stdafx.h"
#include "ExpHelper.h"
#include "tinyxml.h"
#include "CIdMaker.h"
#include "CCypherVariantCodeTable.h"
#include "TSqrAllocator.inl"
#include "TraceHelper.h"

CBaseCypherVariantCode::CBaseCypherVariantCode(uint32 uID,const char* szName, CBaseCypherVariantCode *pParent)
:m_uID(uID), m_szName(szName), m_pParent(pParent)
{
	//if ( !szName )
	//	return;

	//m_szName = new char[strlen(szName) + 1];
	//strncpy(m_szName, szName, strlen(szName) + 1);	
}

CBaseCypherVariantCode::~CBaseCypherVariantCode()
{
	for (NameMapType::iterator it = m_NameMap.begin(); it != m_NameMap.end(); ++it)
	{
		it->second->m_pParent = NULL;
	}

	//delete[] m_szName;
}

CBaseCypherVariantCode* CBaseCypherVariantCode::GetEntity(const char* szName) const
{
	NameMapType::const_iterator it = m_NameMap.find(szName);
	
	if (it == m_NameMap.end())
		return NULL;
	
	return it->second;
}

size_t CBaseCypherVariantCode::GetChildrenCount()const
{
	Ast(this);
	return m_NameMap.size();
}

uint32 CBaseCypherVariantCode::GetIDByPath(const char* szPath) const
{
	string s(szPath);
	vector<string> vect;

	string::size_type begin = 0;
	string::size_type end = s.find('/', begin); 
	string temp = s.substr(begin, end - begin);

	if (!temp.empty())
		vect.push_back(temp);

	while (end != string::npos)
	{    
		begin = ++end;
		end = s.find('/', begin);
		string temp = s.substr(begin, end - begin);
		if (!temp.empty())
			vect.push_back(temp);   
	}

	const CBaseCypherVariantCode *pCode = this;
	for (vector<string>::iterator it = vect.begin(); it != vect.end(); ++it)
	{
		pCode = pCode->GetEntity(it->c_str());
		if (pCode == NULL)
		{
			ostringstream strm;
			strm << "Invalid Variant Path:" << szPath;
			GenErr(strm.str());
		}

	}
	return pCode->GetID();
}

CCypherVariantCode::CCypherVariantCode(uint32 uID, const char* szName, CCypherVariantCode *pParent)
:CBaseCypherVariantCode(uID, szName, pParent)
,m_uFirstChildID(uint32(-1))
{
}

CCypherVariantCode::~CCypherVariantCode()
{
}

void CCypherVariantCode::AddEntity(CBaseCypherVariantCode *pEntity)
{
	if ( !m_NameMap.insert(std::make_pair(pEntity->GetName(), pEntity)).second )
		GenErr("CBaseCypherVariantCode duplicated!");
	
	if( m_IDMap.empty() )
		m_uFirstChildID = pEntity->GetID();

	pEntity->m_uIndex = (uint32)m_IDMap.size();
	m_IDMap.push_back( pEntity );
}

CBaseCypherVariantCode* CCypherVariantCode::GetEntity(uint32 uID) const
{
	const uint32 uPos = uID - m_uFirstChildID;
	if( uPos > m_IDMap.size() )
		GenErr("Invalid Variant Code!");
	return m_IDMap[ uID ];
}

//////////////////
CCypherVariantDynCode::CCypherVariantDynCode(uint32 uID, 
	 const char* szName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent)
:CBaseCypherVariantCode(uID, szName, pParent)
,m_bIsClone(false)
,m_bHasDynChildCode(bHasDynChildCode)
{
	//m_vecSubTreeNode.resize(256);
}

CCypherVariantDynCode::CCypherVariantDynCode(uint32 uID, 
	const char* szName, const char* szNewName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent)
	:CBaseCypherVariantCode(uID, szName, pParent)
	,m_bIsClone(false)
	,m_bHasDynChildCode(bHasDynChildCode)
{
	//m_vecSubTreeNode.resize(256);
	//GetParent()->m_NameMap.insert(std::make_pair(szNewName, this));
//	pParent->m_NameMap.insert(std::make_pair(szNewName, this));
}

CCypherVariantDynCode::~CCypherVariantDynCode()
{
	if (m_pParent && (m_pParent->HasDynChildCode()))
		m_pParent->DelEntity(m_szName.c_str());	

	for (NameMapType::iterator it = m_NameMap.begin(); it != m_NameMap.end();)
	{
		CBaseCypherVariantCode* pCode = (it++)->second;
		pCode->m_pParent = NULL;
		//DelSubTree(class_cast<CCypherVariantDynCodeForEncode*>(pCode));
	}
}

//void CCypherVariantDynCode::DelSubTree(CCypherVariantDynCodeForEncode* pCode)
//{
//	for (NameMapType::iterator it = pCode->m_NameMap.begin(); it != pCode->m_NameMap.end();)
//	{
//		DelSubTree(class_cast<CCypherVariantDynCodeForEncode*>((it++)->second));
//	}
//	if (pCode->m_NameMap.empty())
//	{
//		delete pCode;
//	}
//}

void CCypherVariantDynCode::AddEntity(CCypherVariantDynCode* pEntity, uint32 slot)
{
	if ( !m_NameMap.insert(std::make_pair(pEntity->GetName(), pEntity)).second )
		GenErr("CBaseCypherVariantCode duplicated!");

	pEntity->m_uSlot = slot;
	//m_vecSubTreeNode[slot] = pEntity;
	pEntity->m_uIndex = slot;
}

//CBaseCypherVariantCode* CCypherVariantDynCode::GetEntity(uint32 uId)const
//{
//	if (uId <= m_vecSubTreeNode.size())
//		return m_vecSubTreeNode[uId];
//	return NULL;
//}

CBaseCypherVariantCode* CCypherVariantDynCode::GetEntity(const char* szName)const
{
	return CBaseCypherVariantCode::GetEntity(szName);
}

uint32 CCypherVariantDynCode::IntDelEntity(const char* szName)
{
	CCypherVariantDynCode* pCode;
	CBaseCypherVariantCode::NameMapType::iterator it = m_NameMap.find(szName);
	if (it != m_NameMap.end())
	{
		CBaseCypherVariantCode* pVarCode = it->second;
		pCode = class_cast<CCypherVariantDynCode*>(pVarCode);
		m_NameMap.erase(it);
		//m_vecSubTreeNode[pCode->m_uSlot] = NULL;
		return pCode->m_uSlot;
	}
	return uint32(-1);
}

void CCypherVariantDynCode::DelEntity(const char* szName)
{
	IntDelEntity(szName);
}

CCypherVariantDynCode* CCypherVariantDynCode::clone(CBaseCypherVariantCode* pParent)
{
	CCypherVariantDynCode* pCode = new CCypherVariantDynCode(GetID(), GetName(), m_bHasDynChildCode, pParent);
	pCode->m_bIsClone = true;
	pCode->m_uIndex = m_uIndex;
	return pCode;
}

void CCypherVariantDynCode::clear()
{
	for (NameMapType::iterator it = m_NameMap.begin(); it != m_NameMap.end();)
	{
		//delete code
		delete (it++)->second;
	}
	//m_vecSubTreeNode.clear();
	//m_vecSubTreeNode.resize(256);
	m_NameMap.clear();
}

void CCypherVariantDynCode::InterpretSubTreePath(uint32 uFullPath, uint32& uParentIndex, uint32& uSubTreeIndex, uint32& uSubTreeType, uint32& uIndex)
{
	uParentIndex = uFullPath >> 24;
	uSubTreeIndex = (uFullPath & 0xFF0000) >> 16;
	uSubTreeType = (uFullPath & 0xFF00) >> 8;
	uIndex = (uFullPath & 0xFF);
}

uint32 CCypherVariantDynCode::GenerateSubTreePath(uint32 uParentIndex, uint32 uSubTreeIndex, uint32 uSubTreeType, uint32 uIndex)
{
	return (uParentIndex << 24) + (uSubTreeIndex << 16) + (uSubTreeType << 8) + uIndex;
}

CCypherVariantDynCodeForEncode::CCypherVariantDynCodeForEncode(uint32 uID, uint32 uDynamicId, 
	 const char* szName, bool bHasDynChildCode, CBaseCypherVariantCode *pParent)
:CCypherVariantDynCode(uID, szName, bHasDynChildCode, pParent)
,m_uDynamicId(uDynamicId)
{
	//DebugOut("CCypherVariantDynCodeForEncode create %s\t%p\n", szName, this);
}

CCypherVariantDynCodeForEncode::~CCypherVariantDynCodeForEncode()
{
	//DebugOut("CCypherVariantDynCodeForEncode delete %s\t%p\n", m_szName.c_str(), this);
	//cout << "delete encodecode " << this << endl;
}

void CCypherVariantDynCodeForEncode::DelEntity(const char* szName)
{
	IntDelEntity(szName);
}

CCypherVariantDynCodeForEncode* CCypherVariantDynCodeForEncode::clone(CBaseCypherVariantCode* pParent)
{
	CCypherVariantDynCodeForEncode* pCode = new CCypherVariantDynCodeForEncode(GetID(), GetDynamicId(), GetName(), m_bHasDynChildCode, pParent);
	pCode->m_bIsClone = true;
	pCode->m_uIndex = m_uIndex;
	return pCode;
}

////////////////
void CCypherVariantTreeRootCode::AddSubTreeType(const std::string& str)
{
	m_vecSubTreeType.push_back(str);
}

bool CCypherVariantTreeRootCode::ValidateSubTreeType(const std::string& strTreeType)const{
	for (SubTreeType_t::const_iterator it = m_vecSubTreeType.begin();
		it != m_vecSubTreeType.end(); ++it)
	{
		if(*it == strTreeType)
			return true;
	}
	return false;
}

std::string CCypherVariantTreeRootCode::GetSubTreeName(size_t index)const
{
	Ast(index < m_vecSubTreeType.size());
	return m_vecSubTreeType[index];
}

CCypherVariantTreeRootCodeForEncode::CCypherVariantTreeRootCodeForEncode(uint32 uId, uint32 uDynamicId, const char* szName, CBaseCypherVariantCode* pParent)
:CCypherVariantDynCodeForEncode(uId,uDynamicId,szName,true,pParent)
{
	//DebugOut("CCypherVariantTreeRootCodeForEncode create %s\t%p\n", szName, this);
	m_IdMaker = new IdMaker(256);
}

CCypherVariantTreeRootCodeForEncode::~CCypherVariantTreeRootCodeForEncode()
{
	delete m_IdMaker;
	//DebugOut("CCypherVariantTreeRootCodeForEncode delete %s\t%p\n", m_szName.c_str(), this);
}

uint32 CCypherVariantTreeRootCodeForEncode::GetNewChildId()
{
	uint32 slot;
	if (m_IdMaker->GetId(slot))
		return slot;
	GenErr("Err occer!");
	return -1;
}

void CCypherVariantTreeRootCodeForEncode::clear()
{
	delete m_IdMaker;
	m_IdMaker = new IdMaker(256);
	CCypherVariantDynCode::clear();
}

void CCypherVariantTreeRootCodeForEncode::DelEntity(const char* szName)
{
	uint32 uDeletedSlot = CCypherVariantDynCodeForEncode::IntDelEntity(szName);
	if(uDeletedSlot != uint32(-1))
		m_IdMaker->ReleaseId(uDeletedSlot);
}

CCypherVariantDynCodeForEncode* CCypherVariantTreeRootCodeForEncode::clone(CBaseCypherVariantCode* pParent)
{
	CCypherVariantTreeRootCodeForEncode* pCode = new CCypherVariantTreeRootCodeForEncode(GetID(),m_uDynamicId,GetName(),pParent);
	for (SubTreeType_t::iterator it = m_vecSubTreeType.begin();
		it != m_vecSubTreeType.end(); ++it)
	{
		pCode->m_vecSubTreeType.push_back(*it);
	}
	pCode->m_bIsClone = true;
	pCode->m_uIndex = m_uIndex;
	return pCode;
}

CCypherVariantTreeRootCodeForDecode::CCypherVariantTreeRootCodeForDecode(uint32 uId, const char* szName, CBaseCypherVariantCode* pParent)
:CCypherVariantDynCode(uId,szName,true,pParent)
{
	//DebugOut("CCypherVariantTreeRootCodeForDecode create %s\t%p\n", szName, this);
}

CCypherVariantTreeRootCodeForDecode::~CCypherVariantTreeRootCodeForDecode()
{
	//DebugOut("CCypherVariantTreeRootCodeForDecode delete %s\t%p\n", m_szName.c_str(), this);
}

CCypherVariantDynCode* CCypherVariantTreeRootCodeForDecode::clone(CBaseCypherVariantCode* pParent)
{
	CCypherVariantTreeRootCodeForDecode* pCode = new CCypherVariantTreeRootCodeForDecode(GetID(),GetName(),pParent);
	for (SubTreeType_t::iterator it = m_vecSubTreeType.begin();
		it != m_vecSubTreeType.end(); ++it)
	{
		pCode->m_vecSubTreeType.push_back(*it);
	}
	pCode->m_bIsClone = true;
	pCode->m_uIndex = m_uIndex;
	return pCode;
}

////////////////////////////////////////////////////
//example:
//<root>
//	<user>
//		<head/>
//	</user>
//</root>
CCypherVariantCodeTable::CCypherVariantCodeTable()
{
}

CCypherVariantCodeTable::~CCypherVariantCodeTable()
{
	DeleteAll();
}
bool CCypherVariantCodeTable::LoadConfig(istream & strm, istream & strmSubTree, const char* sRootName)
{
	DeleteAll();

	TiXmlDocument doc;
	strm >> doc;

	CCypherVariantCode* pRoot = new CCypherVariantCode( uint32(m_IDMap.size()),"root", NULL);
	
	AddEntity(pRoot);
	
	TiXmlNode* pRootNode = doc.FirstChild(sRootName);
	for (TiXmlNode* pChild = pRootNode->FirstChild(); pChild; pChild = pChild->NextSibling())
		LoadChildNode(*pChild, pRoot);

	TiXmlDocument subTreeDoc;
	strmSubTree >> subTreeDoc;
	TiXmlNode* pSubTreeRoot = subTreeDoc.FirstChild("SubTree");
	if (!pSubTreeRoot)
	{
		return true;
	}

	for (TiXmlNode* pChild = pSubTreeRoot->FirstChild(); pChild; pChild=pChild->NextSibling())
	{
		CCypherVariantCode* pCode = new CCypherVariantCode( uint32(-1) , pChild->Value(), NULL);
		m_DynamicMap.push_back(pCode);
		LoadSubTree(*pChild, pCode);
	}
	Ast(m_DynamicMap.size()<256);

	for (size_t i = 0; i < m_DynamicMap.size(); ++i)
	{
		Ast(class_cast<CCypherVariantCode*>(m_DynamicMap[i])->m_IDMap.size()<256);
	}
	
	return true;
}

bool CCypherVariantCodeTable::LoadSubTree(TiXmlNode& Node, CCypherVariantCode* pParent)
{
	for(TiXmlNode* pChild = Node.FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		CCypherVariantCode* pCode = new CCypherVariantCode( uint32(-1) , pChild->Value(), pParent);
		pParent->AddEntity(pCode);
		m_SubTreeNode.push_back(pCode);
		LoadSubTree(*pChild, pCode);
	}
	return true;
}

bool CCypherVariantCodeTable::SetSubTreeRootInfo(TiXmlNode &Node, CCypherVariantCode* pParent)
{
	CCypherVariantTreeRootCodeForEncode* pCode = new CCypherVariantTreeRootCodeForEncode( uint32(m_IDMap.size()) , 0, Node.Value() , pParent);
	pParent->AddEntity(pCode);
	AddEntity(pCode);
	for(TiXmlNode* pChild = Node.FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElement = pChild->ToElement();
		if (!pElement)
			continue;
		if (pElement->ValueStr() != "Transit")	
		{
			GenErr("Node must be a Transit\n");
		}
		const char* szNodeType = pElement->Attribute("type");
		if (szNodeType && (0==strcmp(szNodeType, "SubTree")))
		{
			pCode->AddSubTreeType(pElement->Attribute("name"));
		}
		else
		{
			GenErr("szNodeType must be equal than \"SubTree\"\n");
		}
	}	
	return true;
}

bool CCypherVariantCodeTable::LoadChildNode(TiXmlNode &Node, CCypherVariantCode* pParent)
{
	const char* szNodeType = Node.ToElement()->Attribute("type");
	if (szNodeType && (0==strcmp(szNodeType,"Transit"))){
		return SetSubTreeRootInfo(Node, pParent);
	}

	CCypherVariantCode* pCode = new CCypherVariantCode( uint32(m_IDMap.size()) , Node.Value() , pParent);
	pParent->AddEntity(pCode);
	AddEntity(pCode);

	for(TiXmlNode* pChild = Node.FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		LoadChildNode(*pChild, pCode);
	}	
	return true;
}

CBaseCypherVariantCode* CCypherVariantCodeTable::GetRoot() const
{
	return GetEntity(0);
}

bool CCypherVariantCodeTable::AddEntity(CBaseCypherVariantCode *pEntity)
{
	m_IDMap.push_back(pEntity);
	return true;
}

CBaseCypherVariantCode* CCypherVariantCodeTable::GetEntity(uint32 uID) const
{
	if( uID >= m_IDMap.size() )
		return NULL;

	return m_IDMap[uID];
}

void CCypherVariantCodeTable::DeleteAll()
{
	for (CodeVec_t::iterator it = m_IDMap.begin(); it != m_IDMap.end(); ++it)
		delete(*it);
	
	m_IDMap.clear();
	
	for (CodeVec_t::iterator it2 = m_DynamicMap.begin(); it2!=m_DynamicMap.end(); ++it2)
		delete(*it2);
	m_DynamicMap.clear();

	for (CodeVec_t::iterator it3 = m_SubTreeNode.begin(); it3!=m_SubTreeNode.end(); ++it3)
		delete(*it3);
	m_SubTreeNode.clear();
}

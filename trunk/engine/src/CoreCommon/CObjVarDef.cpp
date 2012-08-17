#include "stdafx.h"
#include "CObjVarDef.h"
#include "CCypherVariantCodeTable.h"
#include "PkgStream.h"
#include "CXmlConfig.inl"
#include "BaseHelper.h"

static void LoadTiXMLInfo(TiXmlNode* pNode, TiXmlNode* root)
{
	TiXmlElement* pElement = pNode->ToElement();
	if (pElement->ValueStr() == "SubVarStateTransmit")
	{
		if (pElement->FirstChild())
			Ast("±­¾ßÁË" && false);
		string sSubStateName = pElement->Attribute("StateName");
		cerr << sSubStateName << endl;
		TiXmlNode* pSubState = root->FirstChild(sSubStateName);
		Ast(pSubState);
		TiXmlNode* pNewSubState = pSubState->Clone();
		TiXmlNode* pNewNode = pNode->Parent()->InsertAfterChild(pNode, *pNewSubState);
		pNewNode->ToElement()->SetValue(pElement->Attribute("Name"));
		pNode->Parent()->RemoveChild(pNode);
		return;
	}

	for (TiXmlNode* pChild = pNode->FirstChild(); pChild; )
	{
		TiXmlNode* pNode = pChild;
		pChild = pChild->NextSibling();
		if (!pNode->ToElement())
			continue;
		LoadTiXMLInfo(pNode, root);
	}
}

CObjVarDef::CObjVarDef(const char* szFileName)
{
	ipkgstream ifile(L"", szFileName,ios::binary);
 	if( !ifile)
	{
		ostringstream strm;
		strm<<"Can't open file \""<<szFileName<<"\"";
		GenErr(strm.str());
	}

	CXmlConfig *pTiXmlCfg = new CXmlConfig( "root" , ifile );
	ifile.close();

	TiXmlNode* pRoot = pTiXmlCfg->GetRootNode();
	for (TiXmlNode* pNode = pRoot->FirstChild(); pNode; pNode = pNode->NextSibling())
	{
		if (!pNode->ToElement())
			continue;
		
		LoadTiXMLInfo(pNode, pRoot);
	}

	TiXmlNode *pSubTree = NULL;
	pSubTree = pTiXmlCfg->Get<TiXmlElement*>("SubTree");
	
	stringstream strmSyncPart;
	stringstream strmSubTree;
	TiXmlNode *pSyncPart = NULL;
	pSyncPart = pTiXmlCfg->Get<TiXmlElement*>("SyncPart", "staticroot");
	strmSyncPart << *pSyncPart;
	CCypherVariantCodeTable* pStaticSyncCodeTable = new CCypherVariantCodeTable();
	pStaticSyncCodeTable->LoadConfig( strmSyncPart, strmSubTree, "staticroot");
	m_vecSyncCodeTable.push_back(pStaticSyncCodeTable);
	strmSyncPart.clear();
	pSyncPart = pTiXmlCfg->Get<TiXmlElement*>("SyncPart", "agileroot");
	strmSyncPart << *pSyncPart;
	strmSubTree.clear();
	if (pSubTree)
		strmSubTree << *pSubTree;	
	CCypherVariantCodeTable* pAgileSyncCodeTable = new CCypherVariantCodeTable();
	pAgileSyncCodeTable->LoadConfig( strmSyncPart, strmSubTree, "agileroot");
	m_vecSyncCodeTable.push_back(pAgileSyncCodeTable);

	stringstream strmViewPart;
	TiXmlNode *pViewPart = NULL;
	pViewPart = pTiXmlCfg->Get<TiXmlElement*>("ViewPart", "staticroot");
	strmViewPart << *pViewPart;
	CCypherVariantCodeTable* pStaticViewCodeTable = new CCypherVariantCodeTable();
	pStaticViewCodeTable->LoadConfig( strmViewPart, strmSubTree, "staticroot");
	m_vecViewCodeTable.push_back(pStaticViewCodeTable);

	strmViewPart.clear();
	pViewPart = pTiXmlCfg->Get<TiXmlElement*>("ViewPart", "agileroot");
	strmViewPart << *pViewPart;
	CCypherVariantCodeTable* pAgileViewCodeTable = new CCypherVariantCodeTable();
	pAgileViewCodeTable->LoadConfig( strmViewPart, strmSubTree, "agileroot");
	m_vecViewCodeTable.push_back(pAgileViewCodeTable);
	
	m_sFileName=szFileName;

	delete pTiXmlCfg;
	pTiXmlCfg = NULL;
}


CObjVarDef::~CObjVarDef()
{
	ClearVector(m_vecSyncCodeTable);
	ClearVector(m_vecViewCodeTable);
}

const char* CObjVarDef::GetFileName()const
{
	return m_sFileName.c_str();
}

CCypherVariantCodeTable* CObjVarDef::GetSyncCodeTable(ECodeTableIndex eIndex)const
{
	return m_vecSyncCodeTable[eIndex];
}

CCypherVariantCodeTable* CObjVarDef::GetViewCodeTable(ECodeTableIndex eIndex)const
{
	return m_vecViewCodeTable[eIndex];
}

size_t CObjVarDef::GetSyncCodeTableCount()
{
	return m_vecSyncCodeTable.size();
}

size_t CObjVarDef::GetViewCodeTableCount()
{
	return m_vecViewCodeTable.size();
}

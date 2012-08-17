#pragma once 
#include "CCypherVariantHolder.h"
#include "CCypherVariant.h"
#include "CCypherVariantCodeTable.h"

template<typename CypherVariantImp>
CypherVariantImp* CCypherVariantHolder::AcquireRootVariantFromPool(VariantPool* pPool)
{
	CCypherVariant* pVariant = pPool->Pop();
	if(!pVariant)
	{
		pVariant = GetRootVariant();
		CypherVariantImp* pCypherVariantImp = static_cast<CypherVariantImp*>(pVariant);
		this->OnMemberAdded(pCypherVariantImp);

		//pCypherVariantImp->InitAllMember();
		return pCypherVariantImp;
	}

	m_pRootVariant = pVariant;
	this->OnMemberAdded(m_pRootVariant);
	return static_cast<CypherVariantImp*>(pVariant); 
}

//template<typename CypherVariantImp>
//void CCypherVariantHolder::RecycleRootVariant()
//{
//}

template<typename CypherVariantImp>
CCypherVariant* CCypherVariantHolder::IntAddSubTree(CCypherVariant* pRoot, const char* newName, uint32 uSubTreeType)
{
	CCypherVariant* pSubTree = TRootVariantPoolMgr<CypherVariantImp>::GetInst()->GetSubTreePool(uSubTreeType)->Pop();
	if(!pSubTree)
		return CCypherVariantHolder::IntAddSubTree(pRoot, newName, uSubTreeType);

	CBaseCypherVariantCode* pParent = const_cast<CBaseCypherVariantCode*>(pRoot->GetCode());
	CCypherVariantTreeRootCodeForEncode* pCode = static_cast<CCypherVariantTreeRootCodeForEncode*>(pParent);

	uint32 uSubTreeIndex = pCode->GetNewChildId();

	InitSubTree(pRoot, pSubTree, newName, uSubTreeIndex, uSubTreeType);
	return pSubTree;
}

template<typename CypherVariantImp>
CCypherVariant* CCypherVariantHolderForDecode::IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex)
{
	CCypherVariant* pSubTree = TRootVariantPoolMgr<CypherVariantImp>::GetInst()->GetSubTreePool(uSubTreeType)->Pop();
	if(!pSubTree)
	{
		//cout << " BuildSubTree " << uSubTreeIndex << endl;
		return CCypherVariantHolderForDecode::IntAddSubTreeForDecode(pRoot, uSubTreeType, uSubTreeIndex);
	}

	string strSubTreeRootCodeName;
	GetNameFromIndex(strSubTreeRootCodeName, uSubTreeIndex);
	//cout << " InitSubTree " << strSubTreeRootCodeName << endl;
	InitSubTree(pRoot, pSubTree, strSubTreeRootCodeName.c_str(), uSubTreeIndex, uSubTreeType);
	return pSubTree;
}

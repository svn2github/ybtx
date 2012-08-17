#pragma once
#include "CCypherVariant.h"
#include "CCypherVariantCodeTable.h"
#include "TRootVariantPoolMgr.inl"

template<typename CypherVariantImp>
void CCypherVariant::IntRecycleSubTree()
{
	if(m_eType != eVT_Table)
		return;

	if(m_pParent && m_pParent->GetCode()->IsTreeRoot() && m_pParent->GetCode()->IsCloned())
	{
		CBaseCypherVariantCode* pParentCode = const_cast<CBaseCypherVariantCode*>(m_pParent->GetCode());

		CCypherVariantDynCodeForEncode* pChildCode = static_cast<CCypherVariantDynCodeForEncode*>(m_pCode);
		//cout << " RecycleSubTree " << pChildCode->GetName() << pChildCode->IsCloned() << " " << pChildCode->IsTreeRoot() << endl;
		uint32 uSubTreeTransitID;
		uint32 uSubTreeIndex;
		uint32 uSubTreeType;
		uint32 uIndex;
		CCypherVariantDynCode::InterpretSubTreePath(pChildCode->m_uDynamicId, uSubTreeTransitID, uSubTreeIndex, uSubTreeType, uIndex);

		pParentCode->DelEntity(pChildCode->m_szName.c_str());
		Ast(m_pParent->IntGetMemberByIndex(pChildCode->GetIndex()) == this);
		(*m_pParent->GetMemberMap())[pChildCode->GetIndex()] = NULL;
		pChildCode->m_szName.clear();
		pChildCode->m_uDynamicId = uint32(-1);
		pChildCode->m_pParent = NULL;
		m_pParent = NULL;

		TRootVariantPoolMgr<CypherVariantImp>::GetInst()->GetSubTreePool(uSubTreeType)->Push(this);

		return;
	}

	MemberMap_t* pMemberMap = GetMemberMap();
	if(pMemberMap == NULL)
	{
		return;
	}

	for(typename MemberMap_t::iterator it = pMemberMap->begin(); it != pMemberMap->end(); ++it)
	{
		CCypherVariant* pVariant = *it;

		if(pVariant)
			pVariant->RecycleSubTree();
	}
}

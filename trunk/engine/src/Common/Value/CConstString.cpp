#include "stdafx.h"
#include "CConstString.h"
#include "CConstStringPool.h"
#include "CMemPool.h"

CConstString::CConstString(const char* str, CConstStringPool* pPool, bool bForceInsert )
{
	m_str = pPool->GetStr(str, bForceInsert);
}

CConstString::~CConstString()
{

}

CConstStringPool* CConstString::CreatePool()
{
	return new CConstStringPool;
}

void CConstString::DestroyPool(CConstStringPool* pPool)
{
	delete pPool;
}




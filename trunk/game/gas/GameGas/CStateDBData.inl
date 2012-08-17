#include "CStateDBData.h"
#include "BaseHelper.h"

template <class DBDataType, class AllocatorType>
DBDataType* CDBDataVec<DBDataType, AllocatorType>::CreateDBData()				
{
	return new DBDataType;
}		

template <class DBDataType, class AllocatorType>
CDBDataVec<DBDataType, AllocatorType>::~CDBDataVec()							
{
	ClearVector(m_vecDBData);
}






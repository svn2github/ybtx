#pragma once
#include "TBindObject.h"
#include "TCoreObject.h"
#include "TSqrAllocator.inl"
#include "ErrLogHelper.h"
//#include "ExpHelper.h"
//#include "CoreObjectCommon.h"
//#include "CoreCommon.h"
//#include "CLogCoreObject.h"

template<typename ObjectType>
TBindObject<ObjectType>::TBindObject(ObjectType* pObj)
:m_pCoreObj(pObj)
,m_uBindParentID(0)
{
}


template<typename ObjectType>
TBindObject<ObjectType>::~TBindObject()
{
}

template<typename ObjectType>
bool TBindObject<ObjectType>::BindTo(ObjectType* pParent)
{
	if(GetObject()->GetScene() != pParent->GetScene())
	{
		stringstream str;
		str << GetObject()->GetLocalID() << " to " << pParent->GetLocalID() << endl;
		LogErr("绑定对象双方不在同一场景", str.str());
		GetObject()->OnBeforeBindTo(pParent->GetLocalID(), false);
		GetObject()->OnBindTo(pParent->GetLocalID(), false);
		return false;
	}

	//只能绑定到一个父节点
	if(IsBindingObj())
	{
		GetObject()->OnBeforeBindTo(pParent->GetLocalID(), false);
		GetObject()->OnBindTo(pParent->GetLocalID(), false);
		return false;
	}

	//不允许待绑定的父节点的根节点是自身，避免产生环
	ObjectType* pParentRoot = pParent->m_pBindObj->GetRootObject();
	if(pParentRoot == GetObject())
	{
		GetObject()->OnBeforeBindTo(pParent->GetLocalID(), false);
		GetObject()->OnBindTo(pParent->GetLocalID(), false);
		return false;
	}

	GetObject()->OnBeforeBindTo(pParent->GetLocalID(), true);

	//这部分移到GetObject()->OnBeforeBindTo(pParent->GetLocalID(), true);
	//if(GetObject()->GetMovState())
	//	GetObject()->IntStopMoving();

	//先构造向下链再构造向上链
	pParent->m_pBindObj->m_setBindChildID.insert(GetSelfID());
	m_uBindParentID = pParent->GetLocalID();

	//这部分移到GetObject()->OnBindTo(m_uBindParentID, true);
	//CFPos fParentPos;
	//pParent->GetPixelPos(fParentPos);
	//GetObject()->IntSetPosition(fParentPos);

	//cout << "	" << GetObject()->GetLocalID()<< " BindTo " << pParent->GetLocalID() << "\n";
		//pParent->m_uID << " At(" << fParentPos.x << ", " << fParentPos.y << ")\n";

	GetObject()->OnBindTo(m_uBindParentID, true);
	return true;
}

template<typename ObjectType>
bool TBindObject<ObjectType>::BindTo(uint32 uParentID)
{
	ObjectType* pParent = static_cast<ObjectType*>(ObjectType::GetObject(uParentID));

	if(!pParent)
	{
		GetObject()->OnBeforeBindTo(m_uBindParentID, false);
		GetObject()->OnBindTo(uParentID, false);
		return false;
	}

	return BindTo(pParent);
}

template<typename ObjectType>
bool TBindObject<ObjectType>::DisbindFromParent()
{
	if(m_uBindParentID == 0)
	{
		GetObject()->OnBeforeDisbindFromParent();
		GetObject()->OnDisbindFromParent(false);
		return false;
	}

	ObjectType* pParent = static_cast<ObjectType*>(ObjectType::GetObject(m_uBindParentID));
	if(!pParent)
	{
		GetObject()->OnBeforeDisbindFromParent();
		//uint32 uBindParentID = m_uBindParentID;
		m_uBindParentID = 0;
		GetObject()->OnDisbindChildSingle();
		GetObject()->OnDisbindFromParent(false);
		return false;
	}

	GetObject()->OnBeforeDisbindFromParent();

	//避免产生环，先拆向下链再拆向上链

	//SetObject_t::iterator itr = pParent->m_setBindChildID.find(m_uId);
	//if(itr == pParent->m_setBindChildID.end())
	//{
	//	m_uBindParentID = 0;
	//	return false;
	//}

	//cout << "[	]" << GetObject()->GetLocalID() << " DisbindFromParent " << m_uBindParentID <<"\n";
	//	m_uBindParentID << " At(" << this->m_Pos.x << ", " << this->m_Pos.y << ")\n";

	pParent->m_pBindObj->m_setBindChildID.erase(GetObject()->GetLocalID());
	//uint32 uBindParentID = m_uBindParentID;
	m_uBindParentID = 0;

	GetObject()->OnDisbindChildSingle();
	static_cast<ObjectType*>(pParent)->OnDisbindParentSingle();

	GetObject()->OnDisbindFromParent(true);
	return true;
}

template<typename ObjectType>
bool TBindObject<ObjectType>::DisbindChild(uint32 uChildID)
{

	//避免产生环，先拆向下链再拆向上链
	ObjectType* pChild = static_cast<ObjectType*>(ObjectType::GetObject(uChildID));
	if(!pChild)
	{
		GetObject()->OnBeforeDisbindToChild(uChildID);
	}
	else
	{
		GetObject()->OnBeforeDisbindToChild(uChildID);
	}
	m_setBindChildID.erase(uChildID);
	if(!pChild)
	{
		GetObject()->OnDisbindToChild(uChildID, false);
		GetObject()->OnDisbindParentSingle();
		return false;
	}
	pChild->m_pBindObj->m_uBindParentID = 0;

	//cout << "[	]" << this->m_uID << " DisbindChild " <<
	//	uChildID << " At(" << this->m_Pos.x << ", " << this->m_Pos.y << ")\n";

	pChild->OnDisbindChildSingle();
	GetObject()->OnDisbindParentSingle();

	GetObject()->OnDisbindToChild(uChildID, true);
	return true;
}

template<typename ObjectType>
bool TBindObject<ObjectType>::DisbindAllChildren()
{
	//避免产生环，先拆向下链再拆向上链

	if(m_setBindChildID.empty())
	{
		GetObject()->OnBeforeDisbindToAllChild();
		GetObject()->OnDisbindToAllChild(false);
		GetObject()->OnDisbindParentSingle();
		return false;
	}

	GetObject()->OnBeforeDisbindToAllChild();

	//cout << "[	]" << GetObject()->GetLocalID() << " DisbindAllChildren ";

	for(SetUInt32::iterator itr = m_setBindChildID.begin(); itr != m_setBindChildID.end();)
	{
		uint32 uChildID = (*itr);
		m_setBindChildID.erase(itr++);
		ObjectType* pChild = static_cast<ObjectType*>(ObjectType::GetObject(uChildID));

		//cout << uChildID << " ";

		if(!pChild)
		{
			//这里需要考虑子对象是否真的会不存在，如果是则需考虑返回值一律设为true对逻辑线程造成的影响以及对策
			continue;
		}
		pChild->m_pBindObj->m_uBindParentID = 0;
		pChild->OnDisbindChildSingle();
	}

	//cout << "\n";
	//cout << "At(" << this->m_Pos.x << ", " << this->m_Pos.y << ")\n";

	GetObject()->OnDisbindParentSingle();
	GetObject()->OnDisbindToAllChild(true);
	return true;
}


template<typename ObjectType>
bool TBindObject<ObjectType>::IsBindingObj() const
{
	return m_uBindParentID != 0;
}

template<typename ObjectType>
bool TBindObject<ObjectType>::IsBoundObj() const
{
	return !m_setBindChildID.empty();
}

template<typename ObjectType>
bool TBindObject<ObjectType>::IsBindingOrBoundObj() const
{
	return m_uBindParentID !=0 || !m_setBindChildID.empty();
}

template<typename ObjectType>
ObjectType*
TBindObject<ObjectType>::GetRootObject()
{
	//BindTo和DisbindTo函数绝对不允许产生环，否则会造成这里死循环
	ObjectType*  pRet = GetObject();
	for(;;)
	{
		if(pRet->m_pBindObj->m_uBindParentID == 0)
			break;
		ObjectType* pParent = static_cast<ObjectType*>(ObjectType::GetObject(pRet->m_pBindObj->m_uBindParentID));
		if(!pParent)
			break;
		pRet = pParent;
	}
	return pRet;
}

template<typename ObjectType>
bool TBindObject<ObjectType>::BindObjIntSetPosition(const CFPos& PixelPos)
{
	if(!IsBoundObj())
		return true;
	//如果IntSetPositionSingle不会导致同步删除包括自身在内的任何TCoreObject，则可以直接遍历m_setBindChildID，不需要deqBindChild对象
	//cout << GetObject()->GetLocalID() << " BindObjIntSetPosition(" << PixelPos.x << "," << PixelPos.y << ") To ";
	bool bRet = true;
	deque<uint32> deqBindChild;
	for(SetUInt32::iterator itr = m_setBindChildID.begin(); itr != m_setBindChildID.end(); ++itr)
	{
		deqBindChild.push_back(*itr);
	}
	for(deque<uint32>::iterator itr = deqBindChild.begin(); itr != deqBindChild.end(); ++itr)
	{
		ObjectType* pChild = static_cast<ObjectType*>(ObjectType::GetObject(*itr));
		//cout << (*itr) << "(" << pChild << "), ";
		if(pChild)
		{
			bool bChildRet = pChild->IntSetPositionSingle(PixelPos);
			if(!bChildRet)
			{
				bRet = bChildRet;
			}
		}
	}
	//cout << "\n";
	return bRet;
}

template<typename ObjectType>
void TBindObject<ObjectType>::BindObjNotifyPosChangedIfNeed()
{
	if(!IsBoundObj())
		return;
	//如果IntSetPixelPosSingle不会导致同步删除包括自身在内的任何TCoreObject，则可以直接遍历m_setBindChildID，不需要deqBindChild对象
	deque<uint32> deqBindChild;
	for(SetUInt32::iterator itr = m_setBindChildID.begin(); itr != m_setBindChildID.end(); ++itr)
	{
		deqBindChild.push_back(*itr);
	}
	for(deque<uint32>::iterator itr = deqBindChild.begin(); itr != deqBindChild.end(); ++itr)
	{
		ObjectType* pChild = static_cast<ObjectType*>(ObjectType::GetObject(*itr));
		if(pChild)
		{
			pChild->NotifyPosChangedIfNeedSingle();
			//if(eChildRet != eSPR_Success)
			//{
			//	eRet = eChildRet;
			//}
		}
	}
}


#pragma once
#include "TCoreObjAllocator.h"
#include "CPos.h"
#include "CoreObjectDefs.h"

#ifdef GetObject
#undef GetObject
#endif

template<typename Traits>
class TCoreObject;



namespace sqr
{
	template<typename ObjectType>
	class TBindObject
		:public CCoreObjMallocObject
	{
	public:
		typedef set<uint32, less<uint32>, TCoreObjAllocator<uint32> >	SetUInt32;

		TBindObject(ObjectType* pObj);
		virtual ~TBindObject();

		bool BindTo(ObjectType* pParent);
		bool BindTo(uint32 uParentID);
		//void DisbindTo(ObjectType* pParent);
		bool DisbindFromParent();
		bool DisbindChild(uint32 uChildID);
		bool DisbindAllChildren();
		bool IsBindingObj() const;
		bool IsBoundObj() const;
		bool IsBindingOrBoundObj() const;
		uint32 GetBindParentID()			{return m_uBindParentID;}
		SetUInt32& GetBindChildIDSet()	{return m_setBindChildID;}
		uint32 GetSelfID()					{return m_pCoreObj->GetLocalID();}
		bool BindObjIntSetPosition(const CFPos& PixelPos);
		void BindObjNotifyPosChangedIfNeed();

	protected:
		ObjectType* GetObject()const	{return m_pCoreObj;}
		ObjectType* GetRootObject();




		ObjectType*							m_pCoreObj;
		uint32								m_uBindParentID;
		SetUInt32							m_setBindChildID;

	};
 }

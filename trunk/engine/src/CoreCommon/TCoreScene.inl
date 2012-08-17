#pragma once
#include "TCoreScene.h"
#include "TCoreObject.h"
#include "TCoreSceneMgr.h"
#include "CDynamicBarrierInfo.h"
#include "ErrLogHelper.h"

template<typename Traits>
TCoreScene<Traits>::TCoreScene(MetaScene_t* pMetaScene, CoreSceneMgr_t* pMgr)
:ParentMetaPart_t(pMetaScene),ParentMgrPart_t(pMgr),m_pBarrierInfo(NULL)
{
	SQR_TRY
	{
		m_pBarrierInfo = new CDynamicBarrierInfo(GetMetaScene()->GetWidthInGrid(), GetMetaScene()->GetHeightInGrid());
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << " scene name = " << GetMetaScene()->GetSceneName() << " width = " << GetMetaScene()->GetWidthInGrid() << " height = " << GetMetaScene()->GetHeightInGrid();
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
}

template<typename Traits>
TCoreScene<Traits>::~TCoreScene(void)
{
	delete m_pBarrierInfo;
}

template<typename Traits>
typename TCoreScene<Traits>::ImpCoreSceneMgr_t* 
TCoreScene<Traits>::GetCoreSceneMgr()const
{
	return static_cast<ImpCoreSceneMgr_t*>(ParentMgrPart_t::GetWhole());
}

template<typename Traits>
typename TCoreScene<Traits>::ImpMetaScene_t* 
TCoreScene<Traits>::GetMetaScene()const
{
	return static_cast<ImpMetaScene_t*>(ParentMetaPart_t::GetWhole());
}

template<typename Traits>
CPixelPath* TCoreScene<Traits>::CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd
										   ,EFindPathType eFindPathType, EBarrierType eBarrierType, int32 nMaxStep)
{
	return GetMetaScene()->CreatePath( PixelPosBegin, PixelPosEnd,
		eFindPathType, eBarrierType, nMaxStep, m_pBarrierInfo );
}

template<typename Traits>
CPixelPath* TCoreScene<Traits>::CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd
										   ,EFindPathType eFindPathType, EBarrierType eBarrierType)
{
	return GetMetaScene()->CreatePath( PixelPosBegin, PixelPosEnd,
		eFindPathType, eBarrierType, m_pBarrierInfo );
}

template<typename Traits>
CPixelPath* TCoreScene<Traits>::
CreatePath(const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin, const CFPos& PixelPosEnd)
{
	return GetMetaScene()->
		CreatePath(pPathData, uPathDataLen, PixelPosBegin, PixelPosEnd, m_pBarrierInfo);
}

template<typename Traits>
bool TCoreScene<Traits>::IsGridValid(const CPos& GridPos) const
{	
	return (uint32)GridPos.x < GetMetaScene()->GetWidthInGrid() && (uint32)GridPos.y < GetMetaScene()->GetHeightInGrid();
}

template<typename Traits>
void TCoreScene<Traits>::
SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType)
{
	m_pBarrierInfo->SetBarrier(uOwnerID, dynamicBarrier, eBarrierType);
}


template<typename Traits>
bool TCoreScene<Traits>::HasObject()const
{
	return !m_lstObject.empty();
}


template<typename Traits>
typename TCoreScene<Traits>::ImpCoreObject_t*
TCoreScene<Traits>::GetFirstObject()const
{
	if( !HasObject() )
		return NULL;
	return m_lstObject.front();
}

template<typename Traits>
const typename TCoreScene<Traits>::ListObject_t& 
TCoreScene<Traits>::GetAllObject()const
{
	return m_lstObject;
}


template<typename Traits>
void TCoreScene<Traits>::AddObject(ImpCoreObject_t* pObj)
{
	m_lstObject.push_front( pObj );
	pObj->m_itObjectListInScene=m_lstObject.begin();
}

template<typename Traits>
void TCoreScene<Traits>::DelObject(ImpCoreObject_t* pObj)
{
	Ast( pObj->GetScene() == this );

	m_lstObject.erase( pObj->m_itObjectListInScene );
}

template<typename Traits>
EBarrierType TCoreScene<Traits>::GetBarrier( const CPos& GridPos )const
{
	return GetMetaScene()->GetBarrier(GridPos, m_pBarrierInfo);
}


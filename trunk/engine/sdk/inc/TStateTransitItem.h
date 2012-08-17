#pragma once

#include "BaseTypes.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4290)
#endif

namespace sqr
{
	/*
	* =====================================================================================
	*        Class:  TStateTransitItem
	*  Description:  状态跳转项，记录所有状态内部跳转消息与跳转状态之间的索引
	* =====================================================================================
	*/
	template < class Traits >
	class TStateTransitItem
	{
	public:

		/* ====================  LIFECYCLE     ======================================= */
		TStateTransitItem(typename Traits::StateIDType ID)
			:m_ID(ID)
		{
		}/* constructor */

		/* ====================  OPERATIONS    ======================================= */
		bool AddTransit(typename Traits::EventIDType EventID, typename Traits::StateIDType StateID, typename Traits::StateCreatorType Creator)
		{
			return m_TransitMap.insert(std::make_pair(EventID, NextStateWrapper(StateID, Creator))).second;
		}


		typename Traits::StateType* CreateNextState(typename Traits::OwnerType* Owner, typename Traits::EventIDType EventID) const
		{
			typename TransitMapType::const_iterator it = m_TransitMap.find(EventID);
			if (it == m_TransitMap.end())
			{
				return NULL;
			}
			return (*it->second.m_Creator)(Owner, it->second.m_NextID);
		}

		template <class TableType>
		bool CheckValid(TableType* Table) throw(typename Traits::StateIDType)
		{
			for (typename TransitMapType::iterator it = m_TransitMap.begin();
				it != m_TransitMap.end();
				++it)
			{
				if (!Table->GetTransitItem(it->second.m_NextID))
				{
					throw(it->second.m_NextID);
				}
			}
			return true;
		}
		/* ====================  ACCESS        ======================================= */
		inline typename Traits::StateIDType GetID()
		{
			return m_ID;
		}

	private:
		typename Traits::StateIDType m_ID;
		struct NextStateWrapper
		{
			NextStateWrapper(typename Traits::StateIDType NextID, typename Traits::StateCreatorType Creator)
				:m_NextID(NextID), m_Creator(Creator)
			{
			}
			typename Traits::StateIDType m_NextID;
			typename Traits::StateCreatorType m_Creator;
		};
		typedef std::map<typename Traits::EventIDType, NextStateWrapper> TransitMapType;
		TransitMapType m_TransitMap;
	}; /* ----------  end of template class  TStateTransitItem  ---------- */
}

#ifdef _WIN32
#pragma warning (pop)
#endif


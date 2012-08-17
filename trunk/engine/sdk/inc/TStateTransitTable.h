#pragma once

#include "BaseTypes.h"
#include "TStateTransitItem.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4290)
#endif

namespace sqr
{
	/*
	* =====================================================================================
	*        Class:  TStateTransitTable
	*  Description:  状态跳转表，索引所有状态
	* =====================================================================================
	*/
	template < class Traits >
	class TStateTransitTable 
	{
	public:
		typedef TStateTransitTable<Traits> ThisType;
		typedef TStateTransitItem<Traits> ItemType;

		/* ====================  LIFECYCLE     ======================================= */
		TStateTransitTable (typename Traits::StateIDType EntryID, typename Traits::StateCreatorType EntryCreator)
			:m_EntryID(EntryID)
			,m_EntryCreator(EntryCreator)
		{
		}/* constructor */

		~TStateTransitTable()
		{
		}/* destructor */
		bool AddTransit(typename Traits::StateIDType StateID, typename Traits::EventIDType EventID, typename Traits::StateIDType NextStateID, typename Traits::StateCreatorType NextCreator)
		{
			ItemType *item = GetTransitItem(StateID);
			if (item == NULL)
			{
				item = new ItemType(StateID);
				AddEntity(item);
			}
			return item->AddTransit(EventID, NextStateID, NextCreator);
		}

		ItemType *GetTransitItem(typename Traits::StateIDType StateID) const
		{
			return GetEntity(StateID);
		}

		ItemType* GetEntity(typename Traits::StateIDType ID) const
		{
			typename ContainerType::const_iterator it = this->m_Container.find(ID);
			if (it == this->m_Container.end())
			{
				return NULL;
			}
			return it->second;
		}
		bool AddEntity(ItemType* pEntity)
		{
			return m_Container.insert(std::make_pair(pEntity->GetID(), pEntity)).second;
		}

		bool CheckValid() throw(typename Traits::StateIDType)
		{
			if (this->GetTransitItem(m_EntryID) == NULL)
			{
				throw(m_EntryID);
			}
			for (typename ContainerType::iterator it = m_Container.begin(); it != m_Container.end(); ++it)
			{
				if (!it->second->CheckValid(this))
				{
					return false;
				}
			}
			return true;
		}
		void DeleteAll()
		{
			for (typename ContainerType::iterator it = m_Container.begin(); it != m_Container.end(); ++it)
			{
				delete it->second;
			}
			m_Container.clear();
		}

		typename Traits::StateType* CreateEntryState(typename Traits::OwnerType *Owner) const
		{
			return (*m_EntryCreator)(Owner, m_EntryID);
		}

		void SetEntryCreator(typename Traits::StateIDType ID, typename Traits::StateCreatorType Creator)
		{
			m_EntryID = ID;
			m_EntryCreator = Creator;
		}
	private:
		typedef map<typename Traits::StateIDType, ItemType* > ContainerType;
		ContainerType m_Container;
		typename Traits::StateIDType m_EntryID;
		typename Traits::StateCreatorType m_EntryCreator;
	}; /* ----------  end of template class  TStateTransitTable  ---------- */
}

#ifdef _WIN32
#pragma warning (pop)
#endif


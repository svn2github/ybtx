/*
 * =====================================================================================
 *
 *       Filename:  TEntityManager.h
 *
 *    Description:  支持多重索引的Entity管理器
 *
 *        Version:  1.0
 *        Created:  2008年08月19日 13时54分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  廖鑫炜 (Comet), liaoxinwei@linekong.com
 *        Company:  ThreeOGCMan
 *
 * =====================================================================================
 */

#pragma once

#include "BaseDefs.h"

namespace sqr
{

    template < int V >
    class IntToType
    {
    };
    /*
     * =====================================================================================
     *        Class:  TNullIndex
     *  Description:  空索引
     * =====================================================================================
     */
    template < class T, int V>
        class TNullIndex
        {
            public:
                typedef IntToType<V> IndexType;
                /* ====================  LIFECYCLE     ========================== */
                TNullIndex ()
                {
                }/* constructor */

                /* ====================  OPERATIONS     =========================== */
                bool insert(T* pEntity)
                {
                    return true;
                }

                void erase(T* pEntity)
                {
                }

		void clear()
		{
		}
        }; /* -----  end of class  TNullIndex  ----- */

    /*
     * =====================================================================================
     *        Class:  TEntityIndex
     *  Description:  索引，要求Entity有GetID方法
     * =====================================================================================
     */
    template < class T, class IDType, class ContainerType = map<IDType, T*> >
        class TEntityIndex
        {
            public:
                typedef IDType IndexType;

                /* ====================  LIFECYCLE     ============================ */
                TEntityIndex ()                             /* constructor */
                {
                }

            protected:
                T* find(IDType ID) const
                {
                    typename ContainerType::const_iterator it = this->m_Container.find(ID);
                    if (it == this->m_Container.end())
                    {
                        return NULL;
                    }
                    return it->second;
                }

                bool insert(T* pEntity)
                {
                    return this->m_Container.insert(typename ContainerType::value_type(pEntity->GetID(), pEntity)).second;
                }

                void erase(T* pEntity)
                {
                    this->m_Container.erase(pEntity->GetID());
                }

		void clear()
		{
		    this->m_Container.clear();
		}

		size_t size() const
		{
		    return this->m_Container.size();
		}

                template < class FuncType >
                    bool for_each(FuncType func)
                    {
                        for (typename ContainerType::const_iterator it = this->m_Container.begin();
                                it != this->m_Container.end();
                                ++it)
                        {
                            if (!func(it->second)) return false;
                        }
                        return true;
                    }
            private:
                ContainerType m_Container;
        }; /* ----------  end of template class  TEntityIndexBase  ---------- */

    /*
     * =====================================================================================
     *        Class:  TEntityIndex
     *  Description:  索引，针对Name特化，需要Entity提供GetName方法
     * =====================================================================================
     */
    template < class T, class ContainerType>
        class TEntityIndex< T, string, ContainerType>
        {
            public:
                typedef string IndexType;

                /* ====================  LIFECYCLE     =============================== */
                TEntityIndex ()                             /* constructor */
                {
                }

            protected:
                T* find(const string& Name) const
                {
                    typename ContainerType::const_iterator it = this->m_Container.find(Name);
                    if (it == this->m_Container.end())
                    {
                        return NULL;
                    }
                    return it->second;
                }

                bool insert(T* pEntity)
                {
                    return this->m_Container.insert(typename ContainerType::value_type(pEntity->GetName(), pEntity)).second;
                }

                void erase(T* pEntity)
                {
                    this->m_Container.erase(pEntity->GetName());
                }

		void clear()
		{
		    this->m_Container.clear();
		}

		size_t size() const
		{
		    return this->m_Container.size();
		}

                template < class FuncType >
                    bool for_each(FuncType func)
                    {
                        for (typename ContainerType::iterator it = this->m_Container.begin();
                                it != this->m_Container.end();
                                ++it)
                        {
                            if (!func(it->second)) return false;
                        }
                        return true;
                    }
            private:
                ContainerType m_Container;
        }; /* ----------  end of template class  TEntityIndexBase  ---------- */

    /*
     * =====================================================================================
     *        Class:  TIDIndex
     *  Description:  按ID索引
     * =====================================================================================
     */
    template < class T > 
        class TIDIndex : public TEntityIndex<T, uint32 >
    {
    }; /* -----  end of class  TIDIndex  ----- */


    /*
     * =====================================================================================
     *        Class:  TNameIndex
     *  Description:  按Name索引
     * =====================================================================================
     */
    template < class T > 
        class TNameIndex : public TEntityIndex<T, string, map<string, T*> >
    {
    }; /* -----  end of class  TNameIndex  ----- */

    namespace TEntityManagerPrivate
    {
	template < class T >
	bool ForEachEntityDelete(T* Entity)
	{
	    delete Entity;
		return true;
	}
    };
    /*
     * =====================================================================================
     *        Class:  TEntityManager
     *  Description:  实体管理器，支持多重索引
     * =====================================================================================
     */
    template < class T, class Index1 = TIDIndex<T>, class Index2 = TNullIndex<T, 2>, class Index3 = TNullIndex<T, 3> >
        class TEntityManager : public Index1, public Index2, public Index3
    {
        public:

            /* ====================  LIFECYCLE     ======================================= */
            TEntityManager ()                             /* constructor */
            {
            }

            /* ====================  OPERATIONS    ======================================= */
            bool AddEntity(T* pEntity)
            {
                if (Index1::insert(pEntity))
                {
                    if (Index2::insert(pEntity))
                    {
                        if (Index3::insert(pEntity))
                            return true;
                        Index1::erase(pEntity);
                        Index2::erase(pEntity);
                        return false;
                    }
                    Index1::erase(pEntity);
                    return false;
                }
                return false;
            }

            void RemoveEntity(T* pEntity)
            {
                Index1::erase(pEntity);
                Index2::erase(pEntity);
                Index3::erase(pEntity);
            }

            T* GetEntity(typename Index1::IndexType ID) const
            {
                return Index1::find(ID);
            }

            T* GetEntity(typename Index2::IndexType ID) const
            {
                return Index2::find(ID);
            }

            T* GetEntity(typename Index3::IndexType ID) const
            {
                return Index3::find(ID);
            }

	    void DeleteAll()
	    {
		for_each(TEntityManagerPrivate::ForEachEntityDelete<T>);
		Index1::clear();
		Index2::clear();
		Index3::clear();
	    }

	    size_t GetSize() const
	    {
			return Index1::size();
	    }

            template < class FuncType >
                bool for_each(FuncType func)
                {
                    return Index1::for_each(func);
                }
    }; /* ----------  end of template class  TEntityManager  ---------- */


}

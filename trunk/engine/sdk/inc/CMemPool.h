#pragma once
#include <cstdlib>
#include "Memory.h"
#include "ExpHelper.h"

namespace sqr
{
	class CSqrAllocPool;

	class CMemPool
	{
	private:
		typedef vector<char*>	BuffList_t;

		BuffList_t		m_BuffList;
		size_t			m_stTail;
		size_t			m_stDefaultBlockSize;
		CSqrAllocPool*	m_pPool;

	public:
		CMemPool(size_t stBlockSize, CSqrAllocPool* pPool = NULL)
			:m_stTail(0),m_stDefaultBlockSize(stBlockSize)
			,m_pPool(pPool)
		{
			m_BuffList.push_back((char*)SqrNew(m_stDefaultBlockSize, m_pPool));
		}

		inline size_t GetBlockCount()const
		{
			return m_BuffList.size();
		}

		inline size_t GetBlockSize(size_t stIndex)const
		{
			Ast( stIndex < m_BuffList.size() );
			return SqrGetMemSize( m_BuffList[stIndex] );
		}

		inline char* GetBlock(size_t stIndex)const
		{
			Ast( stIndex < m_BuffList.size() );
			return m_BuffList[stIndex];
		}

		inline char* GetCurBlock()const
		{
			return m_BuffList.back();
		}

		inline size_t GetTailPos()const
		{
			return m_stTail;
		}

		void Clear()
		{
			m_stTail=0;
			if( m_BuffList.size() == 1 )
				return;

			BuffList_t::iterator itEnd = m_BuffList.end() - 1;

			for( BuffList_t::iterator it = m_BuffList.begin() ; it != itEnd ; ++it ) 
				SqrDelete( *it, m_pPool );

			m_BuffList.erase(m_BuffList.begin(),itEnd);
		}


		void* Alloc(size_t shift_size)
		{
			if( shift_size + m_stTail > m_stDefaultBlockSize )
			{
				char* pBuff;
				if( shift_size > m_stDefaultBlockSize )
				{
					pBuff = (char*)SqrNew(shift_size, m_pPool);
					m_BuffList.insert(--m_BuffList.end(),pBuff);
				}
				else
				{
					pBuff = (char*)SqrNew(m_stDefaultBlockSize, m_pPool);
					m_BuffList.push_back(pBuff);
					m_stTail = shift_size;
				}
				return pBuff;
			}
			else
			{
				char *pBuf = m_BuffList.back()+m_stTail;
				m_stTail += shift_size;
				return pBuf;
			}
		}
		~CMemPool()
		{
			Clear();
			SqrDelete( m_BuffList.front(), m_pPool );
		}
	};


}

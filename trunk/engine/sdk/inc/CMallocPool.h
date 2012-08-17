#pragma once
#include "CMallocObject.h"
#include "SqrAlloc.h"

namespace sqr
{

	class CMallocPool
		: public CMallocObject
	{
	private:
		struct Buffer
		{
			Buffer(size_t stSize, char* buffer): m_stSize(stSize), m_pBuffer(buffer){}
			size_t m_stSize;
			char*  m_pBuffer;	
		};

		typedef vector<Buffer, TMallocAllocator<char*> >		BuffList_t;

		BuffList_t		m_BuffList;
		size_t			m_stTail;
		size_t			m_stBlockSize;
	public:
		CMallocPool(size_t size):m_stTail(0),m_stBlockSize(size)
		{
			Buffer buffer(size, (char*)SqrMalloc(sizeof(char) * size));
			m_BuffList.push_back(buffer);
		}

		size_t GetBlockSize()const
		{
			return m_stBlockSize;
		}

		void Clear()
		{
			m_stTail = 0;
			if( m_BuffList.size() == 1 )
				return;

			BuffList_t::iterator itEnd = m_BuffList.end() - 1;

			for( BuffList_t::iterator it = m_BuffList.begin() ; it != itEnd ; ++it ) 
			{
				Buffer buffer = *it;
				SqrFree( buffer.m_pBuffer );
			}	
			m_BuffList.erase(m_BuffList.begin(),itEnd);
		}


		void* Alloc(size_t shift_size)
		{
			if(shift_size > m_stBlockSize || shift_size + m_stTail > m_stBlockSize)
			{
				size_t new_size = shift_size > m_stBlockSize ? shift_size : m_stBlockSize;

				Buffer buffer(new_size, (char*)SqrMalloc(sizeof(char) * new_size));
				m_BuffList.push_back(buffer);

				m_stTail = shift_size;
				return buffer.m_pBuffer;
			}
			else
			{
				Buffer buffer = m_BuffList.back();
				char *pBuf = buffer.m_pBuffer+m_stTail;
				m_stTail += shift_size;
				return pBuf;
			}
		}

		~CMallocPool()
		{
			Clear();
			Buffer buffer = m_BuffList.front();
			SqrFree(buffer.m_pBuffer);
		}
	};

}


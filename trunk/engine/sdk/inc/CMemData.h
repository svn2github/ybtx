#pragma once

namespace sqr
{
	class CSqrAllocPool;

	class CMemData
	{
	public:
		inline CMemData(CSqrAllocPool* pPool);
		inline CMemData(const CMemData& Buffer);
		inline ~CMemData(void);

		inline void Reserve( size_t stSize );
		inline size_t Capacity()const;
		inline void ReSize( size_t stSize );
		inline size_t Size()const;
		inline bool Empty()const;
		inline void Clear();

		inline char* Data();
		inline const char* Data()const;

		inline char* AppendPos();

		//按字节索引偏移，返回指定类型的指针
		template<typename DataType>
		inline DataType* AsPtr(size_t stByteOffset=0);

		//按字节索引偏移，返回指定类型的指针
		template<typename DataType>
		inline const DataType* AsPtr(size_t stByteOffset=0)const;

		//按数据类型大小来进行索引偏移，返回指定类型的数值引用
		template<typename DataType>
		inline DataType& Value(size_t stDataTypeOffset);

		//按数据类型大小来进行索引偏移，返回指定类型的数值引用
		template<typename DataType>
		inline const DataType& Value(size_t stDataTypeOffset)const;


		inline void Trim();
		inline void Assign( const void* pData , size_t stSize );
		inline void Assign( size_t stOffset,const void* pData , size_t stSize );
		inline void Append( const void* pData , size_t stSize );
		inline void Erase( size_t stOffset, size_t stSize );
	private:
		CSqrAllocPool*const	m_pPool;

		size_t	m_stSize;
		size_t	m_stMemSize;
		void*	m_pBuffer;
	};

}


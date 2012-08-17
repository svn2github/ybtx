#pragma once
#include "CFourCC.h"
namespace sqr
{
/*
封装了一个内存快,保存一个指针指向的内存,以及这段内存的大小 eg:[name][size][data].
@备注
这是一个对一段内存块封装的类.
它同时也包含了这段内存块的大小(size)以及简单的标示符(name)
@注意
默认的 DataChunk 的对象析构时<不>会释放任何内存，可以通过释放函数对其进行释放。
@备注
这样做（不在析构的时候释放内存）是为了你打算把它拷贝到其它地方的时候得到更高的效率，
并且这样也保证了DataChunk的安全的拷贝（付值），
eg:你可以放心将DataChunk的对象作为函数的返回值，不必担心它的内存被释放掉。
在你想要释放他的内存的时候你可以调用DataChunk::free
@备注
如果你需要一个可以在析构的时候释放内存的DataChunk，请用SDataChunk代替 -> 现在尚未支持 ->也许我们用不到

*/
class CDataChunk
{
public:
	struct ChunkHdr
	{
		ChunkHdr()
			: dwName(0)
			, ver(0)
			, type(0)
			, stSize(0)
		{
		}

		FourCC	dwName;
		uint16	ver;
		uint16	type;
		size_t	stSize;
	};

	enum EDataChunkTypeMask
	{
		DCT_DEPEND		= 0x0001,
		DCT_AUTO		= 0x0002,
		DCT_LEAF		= 0x0003,
		DCT_REFERENCE	= 0x0004,
	};

public:
	CDataChunk();
	CDataChunk( void* pData,size_t fSize );
	~CDataChunk();

	//char*		allocate( size_t size, const char* ptr = NULL );//重新分配数据
	char* 		SetBuffer( char* ptr,size_t size );
	//void		free();
	char* 		GetBuffer( void );
	const char* GetBuffer( void ) const;
	size_t		GetSize( void )const;
	FourCC		GetName( void )const;
	DWORD		SetName( FourCC fName );
	DWORD		SetName( const std::string& s );
	uint16		GetVersion(void) const;
	void		SetVersion(uint16 ver);
	void		SetType(uint16 type);
	uint16		GetType() const;

	ChunkHdr&	GetHdr(void) const;

	CDataChunk& operator = (CDataChunk& fData );

protected:
	mutable ChunkHdr	m_Header;
	mutable char* 		m_pBuffer;
	mutable char* 		m_pPos;
	mutable	char* 		m_pEnd;
private:
};
}

#pragma once
#include "CRefObject.h"
#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"
#include "CSceneMallocObject.h"

namespace sqr
{
extern void*	BakeStlAlloc( uint32 size );

DeclareAllocObject(CBakeMallocObject);
//DeclarePoolAllocator(CBakeMallocAllocator, CBakeMallocObject);

template<typename T> 
class CBakeStlAllocator
{ 
public:
	typedef size_t    size_type; 
	typedef ptrdiff_t difference_type; 
	typedef T*        pointer; 
	typedef const T*  const_pointer; 
	typedef T&        reference; 
	typedef const T&  const_reference; 
	typedef T         value_type; 

	CBakeStlAllocator() throw() {};

	CBakeStlAllocator( const CBakeStlAllocator& ) throw() {};

	template<class U> 
	CBakeStlAllocator( const CBakeStlAllocator<U>& ) throw() {};

	~CBakeStlAllocator() throw() {}; 

	template<class U> 
	struct rebind 
	{ 
	public:
		typedef CBakeStlAllocator<U> other; 
	};

	bool operator==( const CBakeStlAllocator<T>& )const throw() { return true; };

	pointer allocate( size_type n )
	{
		return reinterpret_cast<pointer>(BakeStlAlloc(n*sizeof(T)));
	}

	void deallocate( pointer p, size_type n )
	{
	}

	size_type max_size() const
	{
		return size_type(size_type(-1) / sizeof(value_type));
	}

	void construct( pointer p, const value_type& value )
	{
		new(p) value_type(value);
	}

	void destroy( pointer p)
	{
		p->~T();
	}
};

template<class _Kty,
class _Ty,
class _Pr = less<_Kty> >
class BakeMap :public map<  _Kty, _Ty, _Pr, CBakeStlAllocator<pair<const _Kty, _Ty> > >
{};

template<class _Ty>
class BakeVector :public vector<  _Ty , CBakeStlAllocator<_Ty> >
{
public:
	typedef std::vector<  _Ty , CBakeStlAllocator<_Ty> > Parent_t;
	BakeVector()
		: Parent_t(){}
	BakeVector(size_t _Count)
		: Parent_t(_Count){}
	BakeVector(size_t _Count, const _Ty& _Val)
		: Parent_t(_Count,_Val){}
};

typedef basic_string<char, std::char_traits<char>,		  CBakeStlAllocator<char> >		BakeString;
typedef basic_string<wchar_t, std::char_traits<wchar_t>,  CBakeStlAllocator<wchar_t> >	BakeWString;

class CPkgFile;
class CVertexHardwareBuffer;
class ShadowBuffer;

struct ColorInfo : public CBakeMallocObject
{
	size_t Offset;
	size_t VerNum;
};

typedef BakeVector<ColorInfo> PieceColor;
struct  ModelColor : public CBakeMallocObject
{
	typedef SString PieceClassName;
	BakeMap<PieceClassName, PieceColor> Map;
};

struct GridColor : public CBakeMallocObject
{
	BakeMap<SString, ModelColor> Map;
};

class CBakeBuffer;

SmartPointer(CBakedColorData);
class CBakedColorData
	: public CRefObject
	, public CBakeMallocObject
{
public:
	CBakedColorData();
	virtual ~CBakedColorData();

	/// 载入数据
	void Load(const string& dir, uint32 Width, uint32 Height);
	/// 清空数据, 释放内存
	void Unload();

	/// 查找指定格子模型颜色集合, 没有返回NULL
	const GridColor*		GetGridColor(uint32 RegionId);
	CVertexHardwareBuffer*	GetColorBuffer(void);
private:
	uint ReadUInt();
	SString ReadString();
	void ReadData(void* data, size_t size);

	void ReadGridColor( uint32 GridIndex, CBakeBuffer& bakeBuf, size_t& size);
	void ReadModelColor(GridColor& outModelColor, CBakeBuffer& bakeBuf, size_t& size);
	void ReadClassColor(ModelColor& outClassColor, CBakeBuffer& bakeBuf, size_t& size);
private:
	CPkgFile* pkgFile;
	typedef BakeMap<uint32,GridColor>		GridColorMap;
	GridColorMap						m_GridColorMap;// hash_map???
};

//------------------------------------------------------------------------------
inline void 
CBakedColorData::Unload()
{
	m_GridColorMap.clear();
}


}// namespace sqr

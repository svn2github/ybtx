//------------------------------------------------------------------------------
//  CBakedColorData.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CBakedColorData.h"
#include "CHardwareBufferMgr.h"
#include "BaseHelper.h"
#include "CPkgFile.h"
#include "CShadowBuffer.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "DebugHelper.h"
#include "CGraphicExp.h"
#include "GraphicBase.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"

const BakeWString	PATH_ALIAS_BAKE = L"res";
const size_t		Block_Size = 1024 * 1024; 
const size_t		BAKE_VERT_BUF_SIZE = 20*1024*1024;

namespace
{
	CVertexHardwareBuffer* g_pColorBuffer = NULL;
}

namespace sqr
{
	DefineAllocObject(CBakeMallocObject);
	template class basic_string<char, char_traits<char>,		CBakeStlAllocator<char> >;
	template class basic_string<wchar_t, char_traits<wchar_t>,	CBakeStlAllocator<wchar_t> >;

	const uint32	BAKE_STL_POOL_INIT_SIZE = 20*1024*1024;
	static void*	g_pBakeStlAllocPool = NULL;
	static uint32	g_uBakeStlPoolOff	= 0;
	struct BakeStlPoolIniter
	{
		BakeStlPoolIniter()
		{
			g_pBakeStlAllocPool = CBakeMallocObject::GetPool()->Alloc(BAKE_STL_POOL_INIT_SIZE);
		}

		~BakeStlPoolIniter()
		{
			CBakeMallocObject::GetPool()->Dealloc(g_pBakeStlAllocPool);
		}
	};
	static BakeStlPoolIniter stlPoolIniter;

	void* BakeStlAlloc( uint32 size )
	{
		if ( g_uBakeStlPoolOff + size > BAKE_STL_POOL_INIT_SIZE ) 
			GenErr("BakeStlPool不够用了");

		void* ret = (uint8*)g_pBakeStlAllocPool + g_uBakeStlPoolOff; 
		g_uBakeStlPoolOff += size;

		return ret;
	}

	void BakeStlReset()
	{
		g_uBakeStlPoolOff = 0;
	}

	class CBakeBuffer
	{
	public:
		CBakeBuffer()
			: m_buf(0)
			, m_off(0)
			, m_size(0)
		{
		}

		~CBakeBuffer()
		{
			SqrDelete(m_buf, CBakeMallocObject::GetPool());
		}

		uint8*	GetBuf() const
		{
			return m_buf;
		}

		uint8*	GetBufForWrite(size_t size)
		{
			if ( m_off + size > m_size )
				reallocBuf(m_size + Block_Size);

			size_t oldOff = m_off;
			m_off += size;
			return m_buf + oldOff;
		}

	private:
		void	reallocBuf(size_t size)
		{
			uint8* pNewBuf = (uint8*)SqrNew(size, CBakeMallocObject::GetPool());
			if ( pNewBuf == 0 )
			{
				uint8* tmp = m_buf;
				m_buf = 0;
				SqrDelete(tmp, CBakeMallocObject::GetPool());
				GenErr("CBakeBuffer申请内存失败，可能由玩家内存不足引起", "");
			}

			if ( m_buf )
			{
				memcpy(pNewBuf, m_buf, m_size);
				SqrDelete(m_buf, CBakeMallocObject::GetPool());
			}
			m_buf = pNewBuf;

			m_size = size;
		}

	private:
		uint8*	m_buf;
		size_t	m_off;
		size_t	m_size;
	};
}


//------------------------------------------------------------------------------
CBakedColorData::CBakedColorData()
: pkgFile(new CPkgFile())
{
	// empty
}

//------------------------------------------------------------------------------
CBakedColorData::~CBakedColorData()
{
	GfkSafeDelete(this->pkgFile);

	BakeStlReset();
}

//------------------------------------------------------------------------------
inline uint
CBakedColorData::ReadUInt()
{
	uint i = 0;
	this->ReadData(&i, sizeof(i));
	return i;
}

//------------------------------------------------------------------------------
inline SString
CBakedColorData::ReadString()
{
	Ast(this->pkgFile->IsValid());

	uint16 length = 0;
	this->ReadData(&length, sizeof(length));
	BakeVector<char> buffer(length + 1, 0);
	this->ReadData(&buffer.front(), length);

	return &buffer.front();
}

//------------------------------------------------------------------------------
inline void
CBakedColorData::ReadData( void* data, size_t size )
{
	this->pkgFile->Read(data, size);
}

//------------------------------------------------------------------------------
void CBakedColorData::Load( const string& dir, uint32 Width, uint32 Height )
{
	if ( !this->m_GridColorMap.empty() )
		return;

	if ( dir.empty() )
		return;

	// open file
	string path = dir + "/baking.cl";
	this->pkgFile->Open( PATH_ALIAS_BAKE.c_str(), path.c_str());
	if (!this->pkgFile->IsValid())
		return;
	
	SQR_TRY
	{
		uint fourcc		= ReadUInt();
		uint version	= ReadUInt();
		if ( 'BKCL' != fourcc || version != 1 )
			GenErr("载入烘培色失败", "标志位或版本号有问题");

		CBakeBuffer bakeBuffer;
		size_t numGrids = ReadUInt() / sizeof(index_t);
		BakeVector<index_t> gridIndices(numGrids, 0);
		ReadData(&gridIndices[0], sizeof(index_t) * gridIndices.size());

		// for each grid
		size_t realSize	= 0;
		for (uint32 gridIndex = 0; gridIndex < gridIndices.size(); ++gridIndex)
		{
			index_t gridOffset = gridIndices[gridIndex];
			if (0 == gridOffset)
				continue;	// no model
			
			if ( this->pkgFile->Tell() != gridOffset )
				GenErr( "载入烘培色失败", "载入烘培色，this->pkgFile->Tell() != gridOffset，可能由玩家内存不足引起" );

			this->ReadGridColor(gridIndex, bakeBuffer, realSize);
		}

		if ( !g_pColorBuffer )
		{
			g_pColorBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(
				sizeof(DWORD),
				BAKE_VERT_BUF_SIZE/sizeof(DWORD),
				HBU_WRITE_ONLY,
				NULL);
	
			if ( !g_pColorBuffer )
			{
				stringstream ss;
				ss << "申请烘培vb失败，申请大小：" << realSize*sizeof(DWORD) << "。";
				GenErr(GraphicErr::GetErrTypeStr(AllocMemory_Err), ss.str());
			}
		}
		g_pColorBuffer->write(0, realSize*sizeof(DWORD), bakeBuffer.GetBuf(), true);
	}
	SQR_CATCH(exp)
	{
		GfkLogPkgErr(exp, path.c_str(), *this->pkgFile );
	}
	SQR_TRY_END
	
	// close file
	this->pkgFile->Close();
}

//------------------------------------------------------------------------------
void CBakedColorData::ReadGridColor( uint32 GridIndex, CBakeBuffer& bakeBuf, size_t& size )
{
	GridColor& modelColor= m_GridColorMap[GridIndex];
	uint fourcc = this->ReadUInt();
	Ast('GRID' == fourcc);
	uint version = this->ReadUInt();
	Ast(1 == version);
	size_t chunkSize = this->ReadUInt();
	Ast(0 != chunkSize);

	// for each model
	size_t numModels = this->ReadUInt();
	for (index_t i = 0; i < numModels; ++i)
	{
		this->ReadModelColor(modelColor,bakeBuf,size);
	}
}

//------------------------------------------------------------------------------
void CBakedColorData::ReadModelColor( GridColor& outModelColor, CBakeBuffer& bakeBuf, size_t& size )
{
	uint fourcc = this->ReadUInt();
	Ast('MODL' == fourcc);
	uint version = this->ReadUInt();
	Ast(1 == version);
	size_t chunkSize = this->ReadUInt();
	Ast(0 != chunkSize);

	// abort 2 uint32s which are used by map editor
	this->pkgFile->Seek(sizeof(uint) * 2, SEEK_CUR);

	SString modelName = this->ReadString();
	ModelColor& classColor= outModelColor.Map[modelName];
	// for each piece class
	size_t numClasses = this->ReadUInt();
	for (index_t i = 0; i < numClasses; ++i)
	{
		this->ReadClassColor(classColor, bakeBuf, size);
	}
}

//------------------------------------------------------------------------------
void CBakedColorData::ReadClassColor( ModelColor& outClassColor, CBakeBuffer& bakeBuf, size_t& size )
{
	uint fourcc = this->ReadUInt();
	Ast('CLAS' == fourcc);
	uint version = this->ReadUInt();
	Ast(1 == version);
	size_t chunkSize = this->ReadUInt();
	Ast(0 != chunkSize);

	SString className = this->ReadString();
	PieceColor& pieceColors = outClassColor.Map[className];
	// for each piece
	size_t numPieces = this->ReadUInt();
	pieceColors.resize(numPieces);
	for (index_t i = 0; i < numPieces; ++i)
	{		
		ColorInfo& colors = pieceColors[i];
		size_t numColors = this->ReadUInt();
		colors.Offset = size*sizeof(DWORD);
		colors.VerNum = numColors;
		size += numColors;

		size_t length = numColors*sizeof(DWORD);
		void* pDstBuffer = bakeBuf.GetBufForWrite(length);
		if ( pDstBuffer )
			this->ReadData( pDstBuffer, length );
	}
}

//------------------------------------------------------------------------------
const GridColor* CBakedColorData::GetGridColor( uint32 gridIndex )
{
	GridColorMap::iterator it = m_GridColorMap.find(gridIndex);
	if (m_GridColorMap.end() == it)
	{
		// not exist
		return NULL;
	}

	return &it->second;
}

CVertexHardwareBuffer* CBakedColorData::GetColorBuffer(void)
{
	return g_pColorBuffer;
}
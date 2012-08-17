#pragma once

namespace sqr
{
	// 数据块的标识的定义
	const uint32	SRGN = 'NGRS';///< Sub Region
	const uint32	RGFG = 'GFGR';///< Flag
	const uint32	BARR = 'RRAB';
	const uint32	GATR = 'RTAG';///< Block
	const uint32	EBLK = 'KLBE';///< Exact Block
	const uint32	TRAP = 'PART';///< Trap
	const uint32	OBJT = 'TJBO';
	const uint32	VHGT = 'TGHV';///< Terrain Height
	const uint32	VDIF = 'FIDV';///< Vertex Diffuse
	const uint32	VSPE = 'EPSV';///< Vertex Specular
	const uint32	VNOR = 'RONV';
	const uint32	VTEX = 'TXET';///< Texture
	const uint32	MODL = 'LDOM';///< Model
	const uint32	LHGT = 'TGHL';///< Logical Height
	const uint32	SBVT = 'TVBS';
	const uint32	GHGT = 'TGHG';
	const uint32	WATR = 'RTAW';///< Water
	const uint32	SCLT = 'TLCS';///< Scene Point Light
	const uint32	SPLT = 'TLPS';///< Scene Spot Light 
	const uint32	AUID = 'DIUA';///< Scene Audio AUID
	const uint32	LOGI = 'IGOL';///< login model
	const uint32	TGMI = 'IMGT';///< terrain grid material index

	enum ESceneUnits
	{
		eSRegionSpan			= 64,											// 区域宽度(以格子为单位)
		eSGridSpan				= 64,											// 格子宽度(最小游戏单位)，图形引擎专用枚举
	};

	// 区域中的数据块的头
	struct RegionChunkHdr
	{
		uint32			nMagicID;		// 标识数据块的类型
		uint32			nOffset;		// 数据块的起始位置相对于文件头的偏移
		uint32			nSize;			// 数据块的大小
	};


	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	enum EUnits
	{
		eRegionSpan			= eSRegionSpan / 4,							// 区域宽度(以格子为单位)
		eGridSpan			= eSGridSpan,								// 格子宽度(最小游戏单位)，图形引擎专用枚举
		eGridSpanForObj		= eGridSpan,								// 格子宽度，对象系统及外层逻辑专用
		eGridNumPerRegion	= eRegionSpan * eRegionSpan,				// 区域格子数
		eVertexNumPerRegion = ( eRegionSpan + 1 )*( eRegionSpan + 1 ),	// 区域顶点数
		eRegDivideNum		= 2,										//一个区域横纵被区分成多少个子区域
		eSubRegNum			= eRegDivideNum * eRegDivideNum,			//一个区域的子区域个数
		eSubRegionSpan		= eRegionSpan / eRegDivideNum,				//子区域宽度(以格子为单位)
	};

	//根据象素坐标得到格子坐标
	inline int32 GetGridByPixel( float fPixelCrd )
	{
		return static_cast<int32>(fPixelCrd) >> 6;// return LogicCrd*128 + Offset;
	}

}

#pragma once
#include "CVector3.h"
#include "CColor.h"
#include "TRefPointer.h"
#include "CSceneCommon.h"
#include "CSceneMallocObject.h"

namespace sqr
{

	class ITexture;
	
	struct CRegionReadData : public CSceneMallocObject
	{
	};

	struct CClientReadData : public CRegionReadData
	{
		CClientReadData()
			: ampRgnInfo(0)
			, argPos(0)
		{

		}

		size_t	ampRgnInfo;
		size_t	argPos;
		//void*	pScene;
	};

	enum ESceneFileType
	{
		ESFT_ARS	= 0,
		ESFT_ARG	= 1,
		ESFT_AMP	= 2,
		ESFT_COUNT,
	};


	struct CREATE_ITEM : public CSceneMallocObject
	{
		uint32				dwResType;		//资源类型:ARA,ARP,ARS,ARE,ARAS		(必填)
		SString				szFileName;		//资源文件名						(必填)
		SString				szClassName;	//资源类名							(必填)
		SString				szParam;		//类型参数
	};

	struct OBJECT_CREATE_PARAM : public CSceneMallocObject
	{
		SVector<CREATE_ITEM> Items;
	};

	class CTerrainSetUnit : public CSceneMallocObject
	{
	public:
		SVector<OBJECT_CREATE_PARAM> m_UnitPart;
	};

	struct	SubRegionData : public CSceneMallocObject
	{
		uint16				m_nSetRegionIndex;		///属于哪个划分区域索引
		CVector3f			m_vCenter;				///中心
		SVector<int>			m_vSubRgnAjenctList;	///相邻区域
		bool				m_bNeedRealCal;			///是否需要实时计算
	};

	class URegionID : public CSceneMallocObject
	{
	public:
		union 
		{	
			struct
			{
				int16	X,Z; 
			};
			uint32	ID;
		};

	public:
		URegionID():ID(0){}
		URegionID(uint32 id):ID(id){}
		URegionID(int16 x, int16 y):X(x),Z(y){}
		
		inline bool operator > (URegionID id) const { return ID>id.ID; };
		inline bool operator < (URegionID id) const { return ID<id.ID; };
		//void operator / (uint32);
		//operator uint32();
	};

	//根据格子坐标和偏移得到浮点象素坐标
	inline float GetFloatPixelOfGrid( int32 LogicCrd, float Offset = 32.0f )
	{
		return static_cast<float>( LogicCrd << 6 ) + Offset;// return LogicCrd*64 + Offset;
	}

	// ========================================================================

	//根据格子索引以及格子顶点相对左下顶点的偏移得到顶点索引
	inline int32 GetRegionVertexIndex( uint8 uRegionGridIndex, int32 nX, int32 nY )
	{
		return ( ( uRegionGridIndex >> 4 ) + nY )*( eRegionSpan + 1 ) + ( uRegionGridIndex&0xf ) + nX;
	}

	//根据格子坐标以及格子顶点相对左下顶点的偏移得到顶点索引
	inline uint8 GetRegionGridIndex( int32 nGridX, int32 nGridY )
	{
		return (uint8)( ( ( nGridY&0xf ) << 4 ) + ( nGridX&0xf ) );
	}

	//根据格子坐标以及格子顶点相对左下顶点的偏移得到区域编号
	inline int32 GetRegionByGrid( int32 nGrid )
	{
		return nGrid >> 4;
	}

	//根据浮点象素坐标得到格子坐标
	inline int32 GetGridByFloatPixel( float fPixelCrd )
	{
		return static_cast<int32>(fPixelCrd) >> 6;// return LogicCrd*64 + Offset;
	}

	//根据浮点象素坐标得到格子内象素偏移
	inline float GetFloatOfsInGrid( float fPixelCrd )
	{
		float fOffset = fPixelCrd - static_cast<int32>(fPixelCrd);
		return (static_cast<int32>(fPixelCrd) & 0x3f) + fOffset;// return LogicCrd*64 + Offset;
	}

	//根据象素坐标得到格子内象素偏移
	inline float GetOfsInGrid( float fPixelCrd )
	{
		float fOffset = fPixelCrd - static_cast<int32>(fPixelCrd);
		return (static_cast<int32>(fPixelCrd) & 0x3f) + fOffset;// return LogicCrd*64 + Offset;
	}
}

#pragma once

#include "FindPathDefs.h"
#include "CDynamicObject.h"
#include "CPos.h"
#include "CFindPathMallocObject.h"


namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	template<typename Traits>
	class TMetaScene;

	//这个类的对象只能由TMetaScene来创建。
	class CPixelPath
		:public virtual CDynamicObject
		,public CFindPathMallocObject
	{
	public:
		void Release();

		CPixelPath* Clone()const;

		const IPath* GetGridPath()const;

		void GetBeginPixelPos(CFPos& PixelPos)const;
		void GetEndPixelPos(CFPos& PixelPos)const;
		size_t GetPathSize()const;

		//该函数对第一个waypoint进行了特殊处理，第一个WayPoint被设置为移动的起点的象素坐标，而不是路径起点的格子坐标经过转换后的象素坐标
		bool GetWayPointPixelPosOfPath(CFPos& PixelPos,uint16 uIndex)const;
		uint16 GetPixelPosOfPath(CFPos& PixelPos,float fDist)const;
		float GetPathPixelLength()const;
		uint16 GetNextWayPointInPixelPos(float fMovedDist, CFPos& posNextWayPoint)const;
		uint16 GetLastWayPointInPixelPos(float fMovedDist, CFPos& posLastWayPoint)const;

		void PrintPath()const;
	private:
		//不管成功还是失败，函数返回后uBeginWayPoint变成搜索时经过的最后一个WayPoint的索引,fDist变成最后一个WayPoint到指定总距离的距离
		//失败的时候,PixelPos没有被改变
		bool GetPixelPosOfPath(CFPos& PixelPos,uint16& uCurWayPoint,float fCurDist,float fDist)const;

		template<typename Traits>
		friend class TMetaScene;

		CFPos	m_BeginPixelPos;
		CFPos	m_EndPixelPos;
		
		IPath*			m_pGridPath;		//移动路径

		CPixelPath(IPath* pPath,const CFPos& BeginPixelPos,const CFPos& EndPixelPos);

		float			m_fPathLength;

		float CalPathPixelLength()const;

		~CPixelPath(void);
	};
}


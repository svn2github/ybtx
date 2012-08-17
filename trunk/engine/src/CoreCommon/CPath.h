#pragma once

#include "IPathFinding.h"
#include <deque>
#include "CFindPathAllocator.h"



namespace sqr
{

	class CMap;


	class CPath
		:public IPath
		,public CFindPathMallocObject
	{
		friend class CTestSuiteMetaSceneServer;
	public:
		CPath(CMap* pMap);
		CPath(const PathList& pathList, CMap* pMap);
		CPath(const int16* pPathData, uint16 uPathDataLen, const CPos& Begin, const CPos& End, CMap* pMap);

		IPath* Clone();
		bool GetPos(CPos& Pos,uint32 uIndex) const
		{
			if(uIndex >= m_Path.size())
				return false;
			Pos = m_Path[uIndex];
			return true;
		}

		void Release();
		void SwapPath(CPath& NewPath);

		const PathList& GetPath()const;

		void ChangeToQuarter();

	private:
		PathList      m_Path;
		CMap*         m_pMap;

		PathList& GetPath();

		// ÄÚÁªº¯Êý
	public:
		inline uint32 GetPathSize()const;
		inline void AddPath(const CPos& Pos);
		inline void PushBackPath(const CPos& Pos);
	};

}


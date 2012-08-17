#pragma once
#include "CDynamicObject.h"
#include "CSceneClassDefine.h"
#include "CDataChunk.h"
#include "TimeHelper.h"
#include "CSceneMallocObject.h"
namespace sqr
{
	class CRegionRecycle : public CSceneMallocObject
	{
	protected:
		struct  RecycleNode;
	public:
		CRegionRecycle();
		void		AddNode(CRegion* fp);
		CRegion*	PopNode(URegionID ID);
		CRegion*	PopNode(void);
		size_t		Size();
		void		OptimizeRecycle(uint32 size);
	protected:
		typedef		SList<RecycleNode>					NodeList;
		typedef		SMap<URegionID,NodeList::iterator>	NodeID_Map;
		NodeID_Map	m_IdMap;
		NodeList	m_RecycleList;	
		uint64		m_FreeTime;
	protected:
	
		struct		RecycleNode : public CSceneMallocObject
		{
			RecycleNode(uint64 ft,CRegion* fp) : time(ft), pRegion(fp) {};
			RecycleNode(CRegion* fp) : time(GetHDProcessTime()),pRegion(fp) {};

			uint64		time;
			CRegion*	pRegion;
			//NodeList::iterator		ListIt;
			NodeID_Map::iterator	IdIt;
		};
	};

	inline size_t
		CRegionRecycle::Size()
	{
		return m_RecycleList.size();
	}

	class CBaseScene 
		: public CSceneMallocObject
		, public virtual CDynamicObject
	{
		friend class CRegion;
	public:
		CBaseScene( const SString& SceneName , CTreeScene* pTreeScene );
		CBaseScene( const SString& SceneName , CTreeScene* pTreeScene , uint16 Wdith , uint16 Height, uint32 RecycleSize );
		virtual ~CBaseScene();
	public:
		bool	SetActivePos(int16 X, int16 Y);

		bool	AddElementManager( CElementManager* pEleMgr );
		bool	DelElementManager( CElementManager* pEleMgr ); //这个函数接口要慎重使用

		void	ClearRecycle(void);
		void	OptimizeRecycle(void);
	
	public:
		CTreeScene*		GetTreeScene();
		uint32			GetWidth() const;
		uint32			GetHeight() const;
		const char* 	GetSceneName() const;
	
	protected:
		SString		m_strSceneName;
		
		uint16		m_ImpWidth;
		uint16		m_ImpHeight;

		uint16		m_ImpHalfWidth;
		uint16		m_ImpHalfHeight;

		uint16		m_uWidth;
		uint16		m_uHeight;

		uint16		m_XPos;
		uint16		m_YPos;

		uint32		m_RecycleSize;
		
		
		typedef		SMap<URegionID,CRegion*>	RegionPool;

		SVector<CElementManager*>	m_vecEleMgrs;
		RegionPool					m_RegionMgr;
		CRegionRecycle				m_RegionRecycle;
		bool						m_isInitialize;
	};

	inline const char* CBaseScene::GetSceneName() const
	{
		return m_strSceneName.c_str();
	}

	inline uint32 CBaseScene::GetWidth() const
	{
		return m_uWidth;
	}

	inline uint32 CBaseScene::GetHeight() const
	{
		return m_uHeight;
	}

	inline CTreeScene*	 CBaseScene::GetTreeScene()
	{
		return (CTreeScene*)m_vecEleMgrs[0];
	}
}
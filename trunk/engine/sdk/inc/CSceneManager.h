#pragma once
#include "CBaseObject.h"
#include "CoreRenderBase.h"
#include "CDataChunk.h"
#include "CCoder.h"
#include "TimeHelper.h"
#include "CSceneMallocObject.h"
namespace sqr
{
	class CSceneCoder : public CCoder
	{
	public:
		CSceneCoder(void);
		virtual bool CSceneManagerDeCode( CSceneManager* pSceneManager, CDataChunk* pDataChunk ) = 0;
	protected:
		bool Lock(CSceneManager* pScene);
		void UnLock(void);
	protected:
		CSceneManager* m_pLockScene;
	};

	class CRegionRecycle
	{
	protected:
		struct  RecycleNode;
	public:
		CRegionRecycle();
		void		AddNode(CRegion* fp);
		CRegion*	PopNode(uint32 ID);
		CRegion*	PopNode(void);
		size_t		Size();
		void		OptimizeRecycle(uint32 size);
	protected:
		typedef		SList<RecycleNode>				NodeList;
		typedef		SMap<uint32,NodeList::iterator>	NodeID_Map;
		NodeID_Map	m_IdMap;
		NodeList	m_RecycleList;	
		uint64		m_FreeTime;
	protected:
	
		struct		RecycleNode
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

	class CBufFile;
	class CSceneManager : public virtual CBaseObject
	{
		friend class CRegion;
	public:
		CSceneManager( const string& SceneName );
		CSceneManager( const string& SceneName , uint32 Wdith , uint32 Height, uint32 RecycleSize );
		virtual ~CSceneManager();
	public:
		bool	SetActivePos(uint32 X, uint32 Y);

		bool	AddElementManager( CElementManager* pEleMgr );
		bool	DelElementManager( CElementManager* pEleMgr ); //这个函数接口要慎重使用

		void	ClearRecycle(void);
		void	OptimizeRecycle(void);
	
	public:
		uint32 GetWidth() const;
		uint32 GetHeight() const;
		const string& GetSceneName() const;
		
	public:
		void	Create( void );
		virtual void	Destroy( void );

	protected:
		virtual void	OnPreLoad(void) {};
		virtual void	OnLoaded(void) {};
	protected:
		string		m_strSceneName;
		
		uint32		m_ImpWidth;
		uint32		m_ImpHeight;

		uint32		m_ImpHalfWidth;
		uint32		m_ImpHalfHeight;

		uint32		m_uWidth;
		uint32		m_uHeight;

		uint32		m_XPos;
		uint32		m_YPos;

		uint32		m_RecycleSize;


		
		
		typedef		SMap<uint32,CRegion*>	RegionPool;

		SVector<CElementManager*>	m_vecEleMgrs;
		RegionPool					m_RegionMgr;
		CRegionRecycle				m_RegionRecycle;
		bool						m_isInitialize;

	protected:
		SVector<CRegionReadData*>	m_vecReadData;
		CDataChunk*					m_FileChunk;
		CSceneCoder*				m_pClientCoder;
	};

	inline const string& CSceneManager::GetSceneName() const
	{
		return m_strSceneName;
	}

	inline uint32 CSceneManager::GetWidth() const
	{
		return m_uWidth;
	}

	inline uint32 CSceneManager::GetHeight() const
	{
		return m_uHeight;
	}
}
#pragma once 

#include "CPos.h"
#include "AoiDef.h"

namespace sqr
{
	class IAoiViewObj;
	class IAoiScene;
	class IAoiViewObjHandler;

	class CSynAoiViewObj;
	class CSynAoiScene;

	class CAoiThreadMgr;

	class CAoiJob
	{
	public:
		void* operator new(size_t stSize);
		void operator delete(void* pMem){}

		void* AllocMem(size_t stSize);
		char* CloneString(const char* szStr);
		void* CloneData(const void* pData, size_t stSize);

		CSynAoiViewObj* GetSynObj(uint32 uId);
		void AddSynObj(uint32 uId, CSynAoiViewObj* pObj);
		void DelSynObj(uint32 uId);
		CSynAoiScene* GetSynScene(uint32 uId);
		void AddSynScene(uint32 uId, CSynAoiScene *pScene);
		void DelSynScene(uint32 uId);

		void Add();

		virtual ~CAoiJob(){}
	};

	class CAoiObjJob : public CAoiJob
	{
	public:
		
	protected:
		CAoiObjJob(uint32 uId) : m_uId(uId) {}
		uint32				m_uId;
	};

	class CAoiObjWithLayerJob : public CAoiObjJob
	{
	public:

	protected:
		CAoiObjWithLayerJob(uint32 uId, uint32 uDimension)
			: CAoiObjJob(uId), m_uDimension(uDimension){}
		uint32 m_uDimension;
	};

	//set size
	class CAoiSetSizeJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiSetSizeJob(uint32 uId, float fSize, uint32 uDimension = 0);
		~CAoiSetSizeJob();
	private:
		float m_fSize;
	};

	//set eye size
	class CAoiSetEyeSightJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiSetEyeSightJob(uint32 uId, float fEyeSight, uint32 uDimension = 0);
		~CAoiSetEyeSightJob ();
	private:
		float m_fEyeSight;
	};

	//set stealth
	class CAoiSetStealthJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiSetStealthJob(uint32 uId, float fStealth, uint32 uDimension = 0);
		~CAoiSetStealthJob();
	private:
		float m_fStealth;
	};

	//set keenness
	class CAoiSetKeennessJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiSetKeennessJob(uint32 uId, float fKeenness, uint32 uDimension = 0);
		~CAoiSetKeennessJob();
	private:
		float m_fKeenness;
	};

	//set view group
	class CAoiSetViewGroupJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiSetViewGroupJob(uint32 uId, uint32 uViewGroup, uint32 uDimension = 0);
		~CAoiSetViewGroupJob();
	private:
		uint32 m_uViewGroup;
	};

	//set grid pos
	class CAoiSetPosJob : public CAoiObjJob
	{
	public:
		CAoiSetPosJob(uint32 uId, const CFPos &pos);
		~CAoiSetPosJob();
	private:
		CFPos		m_Pos;
	};

	class CAoiForceSetPosJob : public CAoiObjJob
	{
	public:
		CAoiForceSetPosJob(uint32 uId, const CFPos &pos);
		~CAoiForceSetPosJob();
	private:
		CFPos	m_Pos;
	};

	class CAoiCreateObjJob : public CAoiObjJob
	{
	public:
		CAoiCreateObjJob(uint32 uId, uint32 uSceneId, uint32 uCoreObjId, const CFPos& pos, EAoiObjType type);
		~CAoiCreateObjJob();
	private:
		uint32		m_uSceneId;
		uint32		m_uCoreObjId;
		CFPos		m_Pos;
		EAoiObjType	m_eType;
	};

	class CAoiDestroyObjJob : public CAoiObjJob
	{
	public:
		CAoiDestroyObjJob(uint32 uId, uint32 uSceneId);
		~CAoiDestroyObjJob();
	private:
		uint32	m_uSceneId;
	};

	class CDimensionInfo;
	class CAoiCreateSceneJob : public CAoiJob
	{
	public:
		CAoiCreateSceneJob(uint32 uSceneId, 
			               const char* szSyncAoiFileName, uint32 uScale, 
			               uint32 uGridWidth, uint32 uGridHeight, CDimensionInfo* DimInfo);
		~CAoiCreateSceneJob();
	private:
		uint32 m_uSceneId;
		uint32 m_uGridWidth;
		uint32 m_uGridHeight;
		uint32 m_uScale;
		char*	m_szSyncAoiFileName;
		CDimensionInfo*	m_DimensionInfo;
	};

	class CAoiDestroySceneJob : public CAoiJob
	{
	public:
		CAoiDestroySceneJob(uint32 uSceneId);
		~CAoiDestroySceneJob();
		
	private:
		int32 m_uSceneId;
	};

	class CAoiPostMsgToVieweeJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiPostMsgToVieweeJob(uint32 uId, const void* pContext, uint32 uDimension);
		~CAoiPostMsgToVieweeJob();
		
	private:
		const void*	m_pContext;
	};

	class CAoiPostMsgToViewerJob : public CAoiObjWithLayerJob
	{
	public:
		CAoiPostMsgToViewerJob(uint32 uId, const void* pContext, uint32 uDimension);
		~CAoiPostMsgToViewerJob();
		
	private:
		const void*	m_pContext;
	};

	class CAoiPostMsgToSynceeJob : public CAoiObjJob
	{
	public:
		CAoiPostMsgToSynceeJob(uint32 uId, const void* pContext);
		~CAoiPostMsgToSynceeJob();
		
	private:
		const void* m_pContext;
	};

	class CAoiPostMsgToSyncerJob : public CAoiObjJob
	{
	public:
		CAoiPostMsgToSyncerJob(uint32 uId, const void* pContext);
		~CAoiPostMsgToSyncerJob();
		
	private:
		const void* m_pContext;
	};

	class CAoiPostMsgToSelfJob : public CAoiObjJob
	{
	public:
		CAoiPostMsgToSelfJob(uint32 uId, const void* pContext);
		~CAoiPostMsgToSelfJob();
		
	private:
		const void* m_pContext;
	};
}


#pragma once 

#include "CCoreServerJob.h"
#include "CPos.h"

namespace sqr
{

	class CSyncMetaSceneServer;

	class CCoreCreateCalcObjJob
		:public CCoreServerJob
	{
	public:
		CCoreCreateCalcObjJob(uint32 uSceneID, uint32 uObjID, const CFPos& PixelPos, uint32 uSlot);
		~CCoreCreateCalcObjJob();
	private:
		uint32 m_uSceneID;
		uint32 m_uObjID;
		CFPos m_PixelPos;
		uint32 m_uSlot;
	};

	class CCoreCreateDicObjJob
		:public CCoreServerJob
	{
	public:
		CCoreCreateDicObjJob(uint32 uSceneID, uint32 uObjID, const CFPos& PixelPos, uint32 uVarDefID, uint32 uSlot);
		~CCoreCreateDicObjJob();
	private:
		uint32 m_uSceneID;
		uint32 m_uObjID;
		CFPos m_PixelPos;
		uint32 m_uVarDefID;
		uint32 m_uSlot;
	};

	class CCoreCreateMedObjJob
		:public CCoreServerJob
	{
	public:
		CCoreCreateMedObjJob(uint32 uSceneID, uint32 uObjID, const CFPos& PixelPos, uint32 uVarDefID, uint32 uSlot, float fDirectorMaxSpeed);
		~CCoreCreateMedObjJob();
	private:
		uint32 m_uSceneID;
		uint32 m_uObjID;
		CFPos m_PixelPos;
		uint32 m_uVarDefID;
		uint32 m_uSlot;
		float m_fDirectorMaxSpeed;
	};

	class CDimensionInfo;
	class CCoreCreateCoreSceneJob
		:public CCoreServerJob
	{
	public:
		CCoreCreateCoreSceneJob(uint32 uSceneID, CSyncMetaSceneServer* pMetaScene,const char* szSyncAoiFileName, CDimensionInfo* DimInfo, uint32 uSlot);
		~CCoreCreateCoreSceneJob();
	private:
		uint32					m_uSceneID;
		CSyncMetaSceneServer*	m_pMetaScene;
		char*					m_sSyncAoiFileName;
		CDimensionInfo*			m_DimensionInfo;
		uint32					m_uSlot;
	};

	class CCoreDestroyCoreSceneJob
		:public CCoreServerJob
	{
	public:
		CCoreDestroyCoreSceneJob(uint32	uSceneID);
		~CCoreDestroyCoreSceneJob();
	private:
		uint32				m_uSceneID;
	};

	class CCoreSendSceneCmdJob
		:public CCoreServerJob
	{
	public:
		CCoreSendSceneCmdJob(uint32 uSceneID, const void* pData, uint32 uSize);
		~CCoreSendSceneCmdJob();
	private:
		uint32			m_uSceneID;
		void*			m_pData;
		uint32			m_uSize;
	};

	class CCoreSetTimeDistortedRatioJob
		:public CCoreServerJob
	{
	public:
		CCoreSetTimeDistortedRatioJob(uint32 uSceneID, float fRatio);
		~CCoreSetTimeDistortedRatioJob();
	private:
		uint32				m_uSceneID;
		float				m_fRatio;
	};

}

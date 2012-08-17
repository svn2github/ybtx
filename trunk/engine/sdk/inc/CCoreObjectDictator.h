#pragma once
#include "CCoreObjectServer.h"
#include "CVariantHolderDef.h"

#ifdef GetObject
#undef GetObject
#endif

namespace sqr
{
	class CSyncCoreObjectDictator;
	class CCoreSceneServer;
	class ISend;
	class CSyncVariantServer;
	class CSyncVariantServerHolder;
	class CObjVarDefCfg;
	class ICoreObjectDictatorHandler;
	class CConnServer;
	class CCoreObjectMediator;
	class CAoiViewChangedResultToLogic;

	template <typename ObjectType>
	class TBindObject;

	class CCoreObjectDictator
		:public CCoreObjectServer
	{
		friend class CCoreSceneServer;
		friend class CCoreObjectMediator;
		friend class CAoiViewChangedResultToLogic;
		friend class TBindObject<CCoreObjectDictator>;
		friend class CBindObjectServer;

		typedef vector<CPos> BarrierArea;
		typedef vector<CSyncVariantServerHolder*,TCoreObjAllocator<CSyncVariantServerHolder*> > VecSyncVariantServerHolder_t;
	public:
		ICoreObjectDictatorHandler* GetHandler()const;
		EServerObjectType GetType()const;
		virtual CCoreObjectMediator* CastToObjMediator();
		virtual CCoreObjectDictator* CastToObjDictator();
		ISend* GetIS(uint32 uRange)const;

		CSyncVariantServer* GetSyncVariant(ECodeTableIndex eIndex)const;
		CSyncVariantServer* GetViewVariant(ECodeTableIndex eIndex)const;
		CSyncVariantServerHolder* GetSyncVariantHolder(ECodeTableIndex eIndex)const;
		CSyncVariantServerHolder* GetViewVariantHolder(ECodeTableIndex eIndex)const;

		static CObjVarDefCfg* GetVarDefCfg();

		virtual void BroadCastForViewAoi(const void* pData,uint32 uSize)const;
		virtual void BroadCastForSyncAoi(const void* pData,uint32 uSize)const;
		void EnableFollowerDelay(bool bEnable);
		static uint32 GetFollowerDelay();
		bool IsFollowerDelayEnabled()const;

		void SetBarrier(float fBarrierSize, EBarrierType eBarrierType);
		bool HasBarrier()const;
		bool GetBarrierType(EBarrierType& eBarrierType)const;
		uint8 GetBarrierSizeInGrid()const;	//因为障碍的设置最小单位为格子,该函数返回的是障碍的格子半径
		float GetBarrierSize()const;

		float Distance(const CCoreObjectDictator* pCoreObj)const;
		void LogMsg(const string& log);

		void SetSyncSize(float fSize);
		void SetSyncStealth( float fStealth);
		void SetSyncGroup(uint32 uGroupID);

		float GetSyncSize()const;
		float GetSyncStealth()const;
		uint32 GetSyncGroup()const;

		virtual bool BindTo(uint32 uParentObjID);
		virtual bool DisbindFromParent();
		virtual bool DisbindChild(uint32 uChildObjID);
		virtual bool DisbindAllChildren();

		static CCoreObjectDictator* GetObject(uint32 uID);


	private:
		void SetRealSyncSize(float fSize);
		void SetRealSyncStealth(float fStealth);
		void SetRealSyncGroup(uint32 uGroupID);

		float GetRealSyncSize()const;
		float GetRealSyncStealth()const;
		uint32 GetRealSyncGroup()const;

		//BindObj引擎线程用来为同步到客户端，同步语义后已不需要发给逻辑线层
		void OnBindTo(uint32 uParentID, bool bRet);			//BindObj逻辑线程用来SetPostion;
		void OnDisbindFromParent(bool bRet) {};
		void OnDisbindToChild(uint32 uChildID, bool bRet) {};
		void OnDisbindToAllChild(bool bRet) {};

		//BindObj给自身派生类用的回调，逻辑线程（修改AOI）专用
		void OnDisbindParentSingle();
		void OnDisbindChildSingle();
		void OnBeforeBindTo(uint32 uParentID, bool bRet);

		//BindObj客户端（预绑定关系）专用
		void OnBeforeDisbindFromParent() {};
		void OnBeforeDisbindToChild(uint32 uChildID) {};
		void OnBeforeDisbindToAllChild() {};


	protected:
		CCoreObjectDictator(ICoreObjectDictatorHandler* pHandler, CCoreSceneServer* pScene, uint32 uVarDefID);
		~CCoreObjectDictator(void);

	private:
		void ClearBarrier();
		void OnViewRelationChanged(CCoreObjectServer* pObjServer,bool bSee, uint32 uDimension);

		void BroadCastForViewAoiExtraConn(const void* pData,uint32 uSize,CConnServer* pExtraConn)const;
		void BroadCastForSyncAoiExtraConn(const void* pData,uint32 uSize, CConnServer* pExtraConn)const;

		void SyncBarrierExtraConn(CConnServer* pExtraConn)const;
		virtual void SyncBarrier()const;
		bool UpdateSceneBarrier(EBarrierType eBarrierType);

		VecSyncVariantServerHolder_t m_vecSyncVariantHolder;
		VecSyncVariantServerHolder_t m_vecViewVariantHolder;

		bool			m_bEnableFollowerDelay;
		float			m_fBarrierSize;
		EBarrierType	m_eBarrierType;

		CBindObjectServer*					m_pBindObj;

	};
}


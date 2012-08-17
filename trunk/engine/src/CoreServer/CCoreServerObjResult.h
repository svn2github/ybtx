#pragma once
#include "CCoreServerResult.h"
#include "CPos.h"


namespace sqr
{
	class CCoreSceneServer;
	class CCoreObjectServer;

	class CCoreServerObjResult
		:public CCoreServerResult
	{
	protected:
		CCoreServerObjResult(uint32 uObjID);
		virtual ~CCoreServerObjResult(void);

		CCoreObjectServer* GetCoreObj()const;
		uint32 m_uObjID;
	};

	class CCoreObjOnMoveBeganResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnMoveBeganResult(uint32 uObjID);
		~CCoreObjOnMoveBeganResult();
	};

	class CCoreObjOnMovePathChangedResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnMovePathChangedResult(uint32 uObjID);
		~CCoreObjOnMovePathChangedResult();
	};

	class CCoreObjOnMoveEndedResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnMoveEndedResult(uint32 uObjID, uint32 uMoveEndedType, uint32 uSessionID);
		~CCoreObjOnMoveEndedResult();

	private:
		uint32 m_uMoveEndedType;
		uint32 m_uSessionID;
	};

	class CCoreObjOnTraceEndedResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnTraceEndedResult(uint32 uObjID, uint32 uTraceEndedType, uint32 uSessionID);
		~CCoreObjOnTraceEndedResult();

	private:
		uint32 m_uTraceEndedType;
		uint32 m_uSessionID;
	};

	class CCoreObjOnTransferedResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnTransferedResult(uint32 uObjID, uint32 uNewSceneID, const CFPos& OldPos);
		~CCoreObjOnTransferedResult();

	private:
		uint32				m_uNewSceneID;
		CFPos				m_OldPos;
	};

	class CCoreObjOnAoiMsgFromSelfResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnAoiMsgFromSelfResult(uint32 uObjID, const void* pContext);
		~CCoreObjOnAoiMsgFromSelfResult();

	private:
		const void* m_pContext;
	};

	class CCoreObjOnEventResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnEventResult(uint32 uObjID, const void* pData);
		~CCoreObjOnEventResult();

	private:
		const void* m_pData;
	};

	class CCoreObjOnCaughtViewSightOfResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnCaughtViewSightOfResult(uint32 uObjID, uint32 uObjGlobalID, uint32 uDimension);
		~CCoreObjOnCaughtViewSightOfResult();

	private:
		uint32	m_uObjGlobalID;
		uint32	m_uDimension;
	};
	class CCoreObjOnLostViewSightOfResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnLostViewSightOfResult(uint32 uObjID, uint32 uObjGlobalID, uint32 uDimension);
		~CCoreObjOnLostViewSightOfResult();

	private:
		uint32	m_uObjGlobalID;
		uint32	m_uDimension;
	};

	class CCoreObjOnDestroyResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnDestroyResult(uint32 uObjID);
		~CCoreObjOnDestroyResult();
	};

	class CCoreObjOnConnectionDisbindedResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnConnectionDisbindedResult(uint32 uObjID);
		~CCoreObjOnConnectionDisbindedResult();
	};

	class CCoreObjOnObserveeEnteredResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnObserveeEnteredResult(uint32 uObjID, uint32 uHandlerID);
		~CCoreObjOnObserveeEnteredResult();
	private:
		uint32 m_uHandlerID;
	};

	class CCoreObjOnObserveeLeftResult : public CCoreServerObjResult
	{
	public:
		CCoreObjOnObserveeLeftResult(uint32 uObjID, uint32 uHandlerID);
		~CCoreObjOnObserveeLeftResult();
	private:
		uint32 m_uHandlerID;
	};

	class CCoreObjWatchResult : public CCoreServerObjResult
	{
	public:
		CCoreObjWatchResult(uint32 uObjID, uint32 uHandlerID, uint32 uWatchResult);
		~CCoreObjWatchResult();

	private:
		uint32 m_uHandlerID;
		uint32 m_uWatchResult;
	};

	class CCoreReSetCoreSceneResult
		:public CCoreServerObjResult
	{
	public:
		CCoreReSetCoreSceneResult(uint32 uObjID, uint32 uSceneID);
		~CCoreReSetCoreSceneResult();
	private:
		uint32			m_uSceneID;
	};

	class CCoreObjEndTransferResult
		:public CCoreServerObjResult
	{
	public:
		CCoreObjEndTransferResult(uint32 uObjID);
		~CCoreObjEndTransferResult();
	};

	class CCoreObjEndDestroyResult
		:public CCoreServerObjResult
	{
	public:
		CCoreObjEndDestroyResult(uint32 uObjID);
		~CCoreObjEndDestroyResult();
	};

	class CCorePixelPosSetResult
		: public CCoreServerObjResult
	{
	public:
		CCorePixelPosSetResult(uint32 uObjID);
		~CCorePixelPosSetResult();
	};

	//BindObj同步语义不需要发给逻辑线程，有回调发送时序逻辑，暂时先别删，以后如果改异步要参考

	//class CCoreObjOnBindToResult : public CCoreServerObjResult
	//{
	//public:
	//	CCoreObjOnBindToResult(uint32 uObjID, uint32 uParentObjID, bool bRet);

	//	~CCoreObjOnBindToResult();
	//private:
	//	uint32 m_uParentObjID;
	//	bool m_bRet;
	//};

	//class CCoreObjOnDisbindFromParentResult : public CCoreServerObjResult
	//{
	//public:
	//	CCoreObjOnDisbindFromParentResult(uint32 uObjID, bool bRet);

	//	~CCoreObjOnDisbindFromParentResult();
	//private:
	//	bool m_bRet;
	//};

	//class CCoreObjOnDisbindToChildResult : public CCoreServerObjResult
	//{
	//public:
	//	CCoreObjOnDisbindToChildResult(uint32 uObjID, uint32 uChildObjID, bool bRet);

	//	~CCoreObjOnDisbindToChildResult();
	//private:
	//	uint32 m_uChildObjID;
	//	bool m_bRet;
	//};

	//class CCoreObjOnDisbindToAllChildResult : public CCoreServerObjResult
	//{
	//public:
	//	CCoreObjOnDisbindToAllChildResult(uint32 uObjID, bool bRet);

	//	~CCoreObjOnDisbindToAllChildResult();
	//private:
	//	bool m_bRet;
	//};

}

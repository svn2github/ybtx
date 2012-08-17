//写在最开始： MD! 这个类实在是太撮了。。。。
//这是一种糟糕的设计，不过现行结构也没有什么更好的方法
#pragma once
#include "CAsynFunParamsMgr.h"
#include "GraphicBase.h"

namespace sqr
{
	/*
	所有CRenderObjcet需要的异步缓冲结构都写到这里面
	*/
	class CRenderObject;
	class CRenderSpaceNode;

	class CRenderObjectFunCommand : public FunCommand
	{
	public:
		CRenderObjectFunCommand( CRenderObject* pRenderObject )
			: m_pObject( pRenderObject ) {};
		virtual ~CRenderObjectFunCommand(){};
		DWORD	GetType(void) { return 'ROFC'; }
	protected:
		CRenderObject*	m_pObject;
	};

	class FunDoAni : public CRenderObjectFunCommand
	{
	public:
		FunDoAni(CRenderObject* pRenderObject,const char* pAction,int32 PartId,bool loop,UINT dtime,int16 sF,int16 eF,float AniSpeed)
			:CRenderObjectFunCommand(pRenderObject),nPartId(PartId),Action(const_cast<char*>(pAction)),bLoop(loop),DelayTime(dtime),nStartFrame(sF),nEndFrame(eF),fAniSpeed(AniSpeed)
		{}
		DWORD	GetType(void) { return stGetType(); }
		bool	Compare(const void* param) { return nPartId == *(const int32*)param; };
		static	DWORD stGetType(void) { return 'FDAI'; }
	protected:
		int32		nPartId;
		SString		Action;
		bool		bLoop;
		UINT		DelayTime;
		int16		nStartFrame;
		int16		nEndFrame;
		float		fAniSpeed;
	protected:
		 virtual bool Do(void);
	};

	class FunShowPiece : public CRenderObjectFunCommand
	{
	public:
		FunShowPiece(CRenderObject* pRenderObject,const char* szPieceName,bool bShow)
			:CRenderObjectFunCommand(pRenderObject),m_szPieceName(const_cast<char*>(szPieceName)),m_bShow(bShow)
		{}
		DWORD	GetType(void) { return stGetType(); }
		bool	Compare(const void* param) { return m_szPieceName == (const char*)param; };
		static	DWORD stGetType(void) { return 'FSPE'; }
	protected:
		SString		m_szPieceName;
		bool		m_bShow;
	protected:
		virtual bool Do(void);
	};

	class FunAddChild : public CRenderObjectFunCommand
	{
	public:
		FunAddChild( CRenderObject* pRenderObject, CRenderSpaceNode* pC,eLinkType lt, const char* szAttachName );
		DWORD	GetType(void) { return stGetType(); }
		bool	Compare(const void* param) { return ((const void*)pChild) == param; }

		static	DWORD stGetType(void) { return 'FACD'; }
	protected:
		CRenderSpaceNode*	pChild;
		eLinkType		linkType;
		SString			szAttachName;
	protected:
		virtual bool Do(void);
	};

	//class FunAddEffect : public CRenderObjectFunCommand
	//{
	//public:
	//	FunAddEffect( CRenderObject* pRenderObject, const char* szFxFile, const char* szFx, const char* szClassName, int32 nPlayTime,  IModelHandler * pHandler)
	//		:CRenderObjectFunCommand(pRenderObject),m_szFxFile(const_cast<char*>(szFxFile)),m_szFx(const_cast<char*>(szFx)),m_szClassName(const_cast<char*>(szClassName)),m_nPlayTime(nPlayTime),m_pHandler(pHandler)
	//	{}
	//	DWORD	GetType(void) { return stGetType(); }

	//	static	DWORD stGetType(void) { return 'FAEF'; }
	//protected:
	//	SString	m_szFxFile;
	//	SString  m_szFx;
	//	SString  m_szClassName;
	//	int32   m_nPlayTime;
	//	IModelHandler * m_pHandler;
	//protected:
	//	virtual bool Do(void);
	//};

	//class FunDeleteNoneActEffect : public CRenderObjectFunCommand
	//{
	//public:
	//	FunDeleteNoneActEffect( CRenderObject* pRenderObject, uint32 mask );
	//	DWORD	GetType(void) { return stGetType(); }

	//	static	DWORD stGetType(void) { return 'DnAe'; }
	//protected:
	//	uint32 nMask;
	//protected:
	//	virtual bool Do(void);
	//};



	class FunSetNextAnimation : public CRenderObjectFunCommand
	{
	public:
		FunSetNextAnimation(CRenderObject* pRenderObject,IDNAME AniName, FramePair FmPair, bool bLoop, int32 DelayTime , float AniSpeed )
			:CRenderObjectFunCommand(pRenderObject),m_AniName(AniName),m_FmPair(FmPair),m_bLoop(bLoop),m_DelayTime(DelayTime),m_AniSpeed(AniSpeed)
		{}
		DWORD	GetType(void) { return stGetType(); }

		static	DWORD stGetType(void) { return 'FSNA'; }
	protected:
		IDNAME		m_AniName;
		FramePair	m_FmPair;
		bool		m_bLoop;
		int32		m_DelayTime;
		float		m_AniSpeed;
	protected:
		virtual bool Do(void);
	};

	class FunSetAnimateGroup: public CRenderObjectFunCommand
	{
	public:
		FunSetAnimateGroup(CRenderObject* pRenderObject,CAnimationGroup* pAniGroup)
			:CRenderObjectFunCommand(pRenderObject),m_pAnimationGroup(pAniGroup)
		{}
		DWORD GetType(void) {return stGetType();}
		static DWORD stGetType(void){return 'FSAG';}
	protected:
		CAnimationGroup*			m_pAnimationGroup;
	protected:
		virtual bool Do(void);
	};

	class FunSetDefaultAniSep:public CRenderObjectFunCommand
	{
	public:
		FunSetDefaultAniSep(CRenderObject* pRenderObject,int32 id, int32 DelayTime , float AniSpeed)
			:CRenderObjectFunCommand(pRenderObject),m_id(id),m_DelayTime(DelayTime),m_AniSpeed(AniSpeed)
		{}
		DWORD GetType(void) {return stGetType();}
		static DWORD stGetType(void){return 'FSDA';}
	protected:
		int32 m_id;
		int32 m_DelayTime;
		float m_AniSpeed;
	protected:
		virtual bool Do(void);
	};

	class FunUpdateAniGrp:public CRenderObjectFunCommand
	{
	public:
		FunUpdateAniGrp(CRenderObject* pRenderObject)
			:CRenderObjectFunCommand(pRenderObject)
		{}
		DWORD GetType(void) {return stGetType();}
		static DWORD stGetType(void){return 'FUAG';}
	protected:
		virtual bool Do(void);
	};


	class FunSetAniSep:public CRenderObjectFunCommand
	{
	public:
		FunSetAniSep(CRenderObject* pRenderObject,IDNAME AniName, int32 id, bool bLoop, int32 DelayTime, float AniSpeed)
			:CRenderObjectFunCommand(pRenderObject),m_AniName(AniName),m_id(id),m_bLoop(bLoop),m_DelayTime(DelayTime),m_AniSpeed(AniSpeed)
		{}
		DWORD GetType(void) {return stGetType();}
		static DWORD stGetType(void){return 'FSAS';}
	protected:
		IDNAME	m_AniName;
		int32	m_id;
		bool	m_bLoop;
		int32	m_DelayTime;
		float	m_AniSpeed;
	protected:
		virtual bool Do(void);
	};

	class FunSetAniFromSep:public CRenderObjectFunCommand
	{
	public:
		FunSetAniFromSep(CRenderObject* pRenderObject,int32 PartID,int32 DelayTime)
			:CRenderObjectFunCommand(pRenderObject),m_PartID(PartID),m_DelayTime(DelayTime)
		{}
		DWORD GetType(void) {return stGetType();}
		static DWORD stGetType(void){return 'FSAF';}
	protected:
		int32	m_PartID;
		int32	m_DelayTime;
	protected:
		virtual bool Do(void);
	};
}
#pragma once
#include "CDynamicObject.h"
#include "CAoiDimFilterType.h"
#include "GameDef.h"
#include "CEntityMallocObject.h"
#include "TEntityAllocator.h"

//struct CAoiDimInfo
//{
//	CAoiDimUnit();
//	CAoiDimUnit(float m_fEyeSight, float m_fSize, float m_fStealth, float m_fKeenness, uint32 m_uGroupID);
//	float	m_fEyeSight;
//	float	m_fSize;
//	float	m_fStealth;
//	float	m_fKeenness;
//	uint32	m_uGroupID;
//};


class CAoiDimFilter;

typedef vector<CAoiDimFilter*, TEntityAllocator<CAoiDimFilter*> >		VecAoiDimFilter;

class CAoiDimMgr;


namespace sqr
{
	class CCoreSceneServer;
	class CCoreObjectServer;
	class CCoreObjectDictator;
	class CCoreObjectMediator;
	class CDimensionInfo;
}

class CAoiDimFilter
	: public virtual CDynamicObject
	, public CEntityMallocObject
{
friend class CAoiDimMgr;
public:
	static void						Unload();
	static CDimensionInfo*			DimensionInfo();
	CAoiDimFilter();
	static uint32					m_uNpcSleepDim;			//Npc的休眠层
private:
	static bool						BuildAoiDimFilter();				//这个以后要做成动态可调配的
	static void						CreateEyeSightFilter(EAoiItemEyeSightType eType);
	static void						CreateSizeFilter(EAoiItemSizeType eType);
	static bool						IsInterestedDim(uint32 eDim)
	{
		return eDim == eADT_VisiPlayerObjInterested || eDim == eADT_InvisiPlayerObjInterested;
	}
	static bool						NeedStealthDim(uint32 eDim);
	static bool						GetEyeSightFilter(EAoiItemEyeSightType uType, uint32 uDim)
	{
		if(uType == eAIET_Undefined) return false;
		return m_VecEyeSightFilter[uType]->m_bDim[uDim];
	}
	static bool						GetSizeFilter(EAoiItemSizeType uType, uint32 uDim)
	{
		if(uType == eAIST_Undefined) return false;
		return m_VecSizeFilter[uType]->m_bDim[uDim];
	}

	void Set(uint32 uDim, bool bSet = true)	{ m_bDim[uDim] = bSet; }
	//bool Get(uint32 uDim)			{ return m_bDim[uDim]; }

	static bool						__init;
	static VecAoiDimFilter			m_VecEyeSightFilter;
	static VecAoiDimFilter			m_VecSizeFilter;
	
	static CDimensionInfo*			m_DimensionInfo;

	deque<bool, TEntityAllocator<bool> >						m_bDim;
};

class CAoiDimMgr
	: public virtual CDynamicObject
	, public CEntityMallocObject
{
public:
	CAoiDimMgr();
	void SetEyeSight(float fEyeSight,CCoreObjectServer* pCoreObj);
	void SetSecondEyeSight(float fInterestedEyeSight,CCoreObjectServer* pCoreObj);
	void SetDoubleEyeSight(float fEyeSight, float fInterestedEyeSight, CCoreObjectServer* pCoreObj);
	void SetSize(float fSize, CCoreObjectServer* pCoreObj);
	void SetStealth(float fStealth, CCoreObjectServer* pCoreObj);
	void SetKeenness(float fKeenness, CCoreObjectServer* pCoreObj);
	void SetViewGroup(uint32 uGroup, CCoreObjectServer* pCoreObj);

	void SetEyeSightType(EAoiItemEyeSightType eType, CCoreObjectServer* pCoreObj);
	void SetSizeType(EAoiItemSizeType eType, CCoreObjectServer* pCoreObj);
	void SetAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType, CCoreObjectServer* pCoreObj);
	void SetObjectAoiType(EObjectAoiType eObjectAoiType, CCoreObjectServer* pCoreObj);
	void UniteObjectAoiType(EObjectAoiType eObjectAoiType, CCoreObjectServer* pCoreObj);

	float GetEyeSight()				{return m_fEyeSight;}
	float GetSecondEyeSight()		{return m_fInterestedEyeSight;}
	float GetSize()					{return m_fSize;}
	float GetStealth()				{return m_fStealth;}
	float GetKeenness()				{return m_fKeenness;}

	
	void InitAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType);

	void InitPlaySizeForNpcSleepDim(CCoreObjectServer* pCoreObj);
	void InitNpcEyeSizeForNpcSleepDim(bool bSleep,CCoreObjectServer* pCoreObj);

private:
	void FlushEyeSight(CCoreObjectServer* pCoreObj);
	void FlushSize(CCoreObjectServer* pCoreObj);
	void FlushStealth(CCoreObjectServer* pCoreObj);
	void FlushKeenness(CCoreObjectServer* pCoreObj);
	void FlushViewGroup(CCoreObjectServer* pCoreObj);
	void FlushOthers(CCoreObjectServer* pCoreObj);
	bool ExistInDim(EAoiDimType eAoiDim);

	EAoiItemEyeSightType			m_eEyeSightType;
	EAoiItemSizeType				m_eSizeType;
	deque<bool, TEntityAllocator<bool> >						m_bEyeSightFlush;
	deque<bool, TEntityAllocator<bool> >						m_bSizeFlush;
	
	EObjectAoiType					m_eObjectAoiType;
	float							m_fEyeSight;
	float							m_fInterestedEyeSight;
	float							m_fSize;
	float							m_fStealth;
	float							m_fKeenness;
	uint32							m_uViewGroup;
};


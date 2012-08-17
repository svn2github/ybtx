#pragma once
#include "CDistortedTick.h"
#include "CNpcAI.h"
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"
 
class CCharacterDictator;
class CNpcAlertMember;

class CNpcAlertMgr
	: public CDistortedTick
	, public CNpcAIMallocObject 
{
typedef	list<CNpcAlertMember*, TNpcAIAllocator<CNpcAlertMember*> >		AlertList;
typedef AlertList::iterator															AlertListIter;
typedef map<uint32, CNpcAlertMember*, less<uint32>, TNpcAIAllocator<pair<uint32, CNpcAlertMember* > > >	AlertMap;
typedef AlertMap::iterator															AlertMapIter;
friend class CNpcAlertMember;
public:
	CNpcAlertMgr(CNpcAI* pNpcAI, uint32 uTargetID, bool bNormal, uint32 uAlertTime);
	~CNpcAlertMgr();

	void OnTick();
	void AddAlertTarget(uint32 uTargetID);
	void RemoveAlertTarget(uint32 uTargetID);
	void OnCanceled(bool bSuccess);
	TPtRefee<CNpcAlertMgr, CNpcAI>& GetRefsByNpcAI() {return m_RefsByNpcAI;}
private:
	CNpcAI*	GetOwner();
	CCharacterDictator* GetAlertTarget();
	CCharacterDictator* GetOwnerCharacter();
	bool IsInAlertList(uint32 uTargetID);
	void OnEnterAlertState();
	void OnLeaveAlertState();
	void UpDateAlertTarget();
	void RegisterDelayAttackTick(uint32 uTargetID);
	void DoNpcAlertEffect(uint32 uAlertTime);
	void DoNpcAlertEndEffect();
	void ClearNpcAlertEffect();
	void AddAlertTargetToMap(uint32 uAlertTargetID, CNpcAlertMember* pMember);
	void DelAlertTargetToMap(uint32 uAlertTargetID, CNpcAlertMember* pMember);
private:
	CNpcAI*							m_pAI;
	AlertList						m_lAlertList;
	AlertMap						m_mAlertMap;
	bool							m_bNormal;
	uint32							m_uAlertTime;	
	bool							m_bAlertEnd;
	TPtRefee<CNpcAlertMgr, CNpcAI>  m_RefsByNpcAI;
};

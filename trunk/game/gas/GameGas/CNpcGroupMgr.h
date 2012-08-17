#pragma once
#include "PatternCOR.h"
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"

class CCharacterDictator;
class CFighterDictator;
class CNpcGroupMgr;

typedef map<uint32, CCharacterDictator* , less<uint32>, TNpcAIAllocator<pair<uint32, CCharacterDictator*> > > MemberIDMapType;

class CNpcGroupMgr 
	: public CPtCORHandler
	, public CNpcAIMallocObject
{
public:
	CNpcGroupMgr(CCharacterDictator* pCharacter);
	~CNpcGroupMgr();

	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void DoAttach(CFighterDictator* pFighter);
	void DoDetach(CFighterDictator* pFighter);

	bool AddMember( CCharacterDictator *pServant );
	void RemoveMember( CCharacterDictator* pServant );
	void RemoveAllMember();
	bool IsInMemberList(uint32 uGlobalID);
	CCharacterDictator* GetMemberByID( uint32 uGlobalID ) const;
	const MemberIDMapType& GetMemberIDMapType() const;
	void SetCharacterHolder(CCharacterDictator* pCharacter);
	inline CCharacterDictator* GetCharacter() const
	{ return m_CharacterHandler.Get(); }

private:
	TPtRefer<CNpcGroupMgr,CCharacterDictator>	m_CharacterHandler;
	MemberIDMapType m_mapMemberByID;
};

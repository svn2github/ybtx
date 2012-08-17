#pragma once
#include "CNpcEventMsg.h"
#include "CNpcAI.h"
#include "CNpcAIMallocObject.h"

//¸¨ÖúÉêÃ÷ºê
#define DECLARE_NPC_Nature(name) \
	class CNpcNature##name : public CNpcNature \
	{ \
		public: \
			CNpcEventMsg* DoEvent(CNpcEventMsg* pEvent); \
			CNpcNature##name(const string & CNatureArg, CNpcAI* pAI): CNpcNature(CNatureArg, pAI){}\
			bool Interest(uint32 uEventID); \
		public:

#define END_DECLARE_NPC_Nature };

#define OVERLOAD_Nature_METHOD(name) CNpcEventMsg* name();

class CNpcNature
	: public CNpcAIMallocObject
{
	protected:
		string m_sArg;
		ENpcEvent m_eEvent;
		uint32 m_uTag;
		uint32 m_uExtraArg;
		uint16 m_uColdTime;
		string m_sAction;
		string m_ActionArg;
		uint16 m_uLevel;
		string m_sNatureAction;
		CNpcAI* m_pAI;
	public:
		CNpcNature(const string & CNatureArg, CNpcAI* pAI);
		virtual ~CNpcNature(){};
	public:
		virtual CNpcEventMsg* DoEvent(CNpcEventMsg* pEvent);
		virtual void SetEvent(ENpcEvent eEvent, uint32 uTag);
		ENpcEvent GetEvent() {return m_eEvent;};
		uint32 GetTag() {return m_uTag;}
		virtual bool Interest(uint32 uEventID) { return false; }
		string& GetArg() { return m_sArg; }
		void SetColdTime(uint16 uColdTime) { m_uColdTime = uColdTime; }
		uint16 GetColdTime()const { return m_uColdTime; }
		void SetExtraArg(uint32 uExtraArg) { m_uExtraArg = uExtraArg; }
		uint32 GetExtraArg() {return m_uExtraArg; }
		void SetAction(const TCHAR* sAction) {m_sAction = sAction;}
		string& GetAction() {return m_sAction;}
		void SetActionArg(const TCHAR* sActionArg) {m_ActionArg = sActionArg;}
		string& GetActionArg() {return m_ActionArg;}
		void SetLevel(uint16 uLevel)  {m_uLevel = uLevel;}
		uint16 GetLevel() const {return m_uLevel;} 
		void SetNatureAction(const TCHAR* szActionName) {m_sNatureAction = szActionName;};
		string& GetNatureAction() {return m_sNatureAction;};
		virtual CNpcEventMsg* EnterState(CNpcEventMsg* pEvent){ return pEvent; }
		virtual CNpcEventMsg* LeaveState(CNpcEventMsg* pEvent){ return pEvent; }
};


DECLARE_NPC_Nature(Attract);
END_DECLARE_NPC_Nature;

DECLARE_NPC_Nature(Like);
END_DECLARE_NPC_Nature;

DECLARE_NPC_Nature(Abhor);
END_DECLARE_NPC_Nature;

DECLARE_NPC_Nature(Coward);
END_DECLARE_NPC_Nature;

DECLARE_NPC_Nature(Sleepy);
END_DECLARE_NPC_Nature;

DECLARE_NPC_Nature(Greet);
END_DECLARE_NPC_Nature;

#undef DECLARE_NPC_Nature
#undef END_DECLARE_NPC_Nature

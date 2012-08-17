#pragma once

class CFighterDictator;
class CEntityServer;
class CCharacterDictator;
class CIntObjServer;

///////////////////////////////////////////////////////////////////////////////////////////////
//	IFighterDictatorHandler类的功能：
//	1.主要是为FighterDictator类提供相关的引擎对象的功能
//	2.提供获取Character和Actor的接口
//	3.为战斗系统内部数值的改变提供回调的接口，（OnAgileValueChanged）
///////////////////////////////////////////////////////////////////////////////////////////////

class IFighterServerHandler
{
public:
	IFighterServerHandler(){m_RefByFighter.SetHolder(this);}
	typedef TPtRefee<IFighterServerHandler,CFighterDictator> RefByFighter;
	RefByFighter& GetRefByFighter(){return m_RefByFighter;}
	virtual ~IFighterServerHandler(){}
	virtual CEntityServer* GetEntity() const = 0;
	virtual CCharacterDictator* GetCharacter() const = 0;
	virtual CIntObjServer* GetIntObj() const = 0;
	virtual uint32 GetEntityID()const=0;

	virtual void OnAgileValueChanged(uint32 uValueTypeID, float fValue) const=0;
	virtual void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const=0;
	virtual void OnDoSkillCtrlStateChanged(uint32 uState, bool bSet)const = 0;
	virtual void OnEnterBattleState()=0;
	virtual void OnLeaveBattleState()=0;
	virtual void OnSkillBegin()const=0;
	virtual void OnSkillEnd(bool bSucceed)const=0;
private:
	RefByFighter	m_RefByFighter;
};


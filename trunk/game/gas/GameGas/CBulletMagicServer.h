#pragma once
#include "CMagicServer.h"

namespace sqr
{
	class CCoreSceneServer;
}

class CBulletMagicCfgServerSharedPtr;
class CGenericTarget;

class CBulletMagicServer
	: public CMagicServer
{	
public:
	CBulletMagicServer(CSkillInstServer* pInst, const CBulletMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CBulletMagicServer(CSkillInstServer* pInst, const CBulletMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos);
	~CBulletMagicServer();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	virtual void OnTraceEnded(ETraceEndedType eTEType);
	virtual void OnMoveEnded(EMoveEndedType eMoveEndedType, uint32 uMoveID);
	virtual void OnMovePathChanged();
	void BulletExplode();
	virtual EMagicGlobalType	GetMagicType() const		{return eMGT_Bullet;}
	CBulletMagicCfgServerSharedPtr&		GetCfgSharedPtr()const;
	void						DestoryBullet();
	void OnDestroy();
	const CFPos& GetBeginPos()const;

private:
	void BecomeDumbBullet();
	void CreatBullet(const CFPos& posFrom);
	void DoEffect();
	static uint32 GetCheckTimes();	// 子弹飞行过程中检查与目标之间距离的次数

	CBulletMagicCfgServerSharedPtr*		m_pCfg;
	CGenericTarget*				m_pTo;
	CCoreSceneServer*			m_pScene;
	bool						m_bIsDumbBullet;	//子弹是否生效, 即是否造成伤害
	bool						m_bIsPos;
	CFPos						m_BeginPos;			//子弹初始位置
};

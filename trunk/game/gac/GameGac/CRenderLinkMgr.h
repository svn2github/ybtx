#pragma once
#include "ActorDef.h"
#include "CTick.h"
#include "CAppClient.h"
#include "CActorMallocObject.h"

namespace sqr
{
	class CRenderObject;
}

class CCharacterFollower;
class CHorseRenderHandler;
class CWeaponRenderHandler;
class COffWeaponRenderHandler;
class CWeaponChangeAniTick;
class CWeaponResumeAniTick;

class CRenderLinkMgr : public CActorMallocObject
{
friend class CHorseRenderHandler;
friend class CWeaponRenderHandler;
friend class COffWeaponRenderHandler;
public:
	CRenderLinkMgr(CCharacterFollower* pEntity);
	~CRenderLinkMgr();

	void InitRenderObject();
	void RemoveRenderObject();

	CRenderObject*		GetRenderChild(const string& strName)const;
	CRenderObject*		GetHorseRenderObj()const{return m_pHorseRenderObj;}
	CRenderObject*		GetMainHandRenderObj()const{return m_pMainHandRenderObj;}
	CRenderObject*		GetOffHandRenderObj()const{return m_pOffHandRenderObj;}
	CRenderObject*		GetAdditionRenderObj()const{return m_pAdditionRenderObj;}

	void		SetWeaponInfo(const string& strMainHandLinkName, const string& strOffHandLinkName,const string& strMainHandLinkEffect,const string& strOffHandLinkEffect);
	void		SetHorseEffect(const string& strHorseEffect);
	void		SetAdditionEffect(const string& strAdditionEffect);
	void		PlayLinkRenderObjEffect(const string& strName,const string& AniName,bool bPlay,bool bLoop);

	float		GetPairScale();
	float		GetScale();
	float		GetFinalScale();
	void		SetScale( float fScale );
	void		SetFinalScale( float fScale, uint64 uFinalTime );

	void		UpdatePos(const CFPos& PixelPos);

	// 武器相关
	void SetWeaponPlayType(EWeaponPlayType eWeaponPlayType);
	void SetOffWeaponPlayType(EWeaponPlayType eWeaponPlayType);
	void SetWeaponVisible(bool bIsMainHand, bool bVisible);
	EWeaponPlayType	GetWeaponPlayType()		{return m_eWeaponPlayType;}
	EWeaponPlayType	GetOffWeaponPlayType()	{return	m_eOffWeaponPlayType;}

	// 播放武器动画
	void PlayWeaponAnimation(string strWeapon, uint8 uType = 0);

	void DoWeaponChangeAni(bool bChange);
	void DoWeaponAnimation(uint8 uType);
	bool WeaponPlayTypeIsBattleChange();

	void OnRenderObjDestroyed();
private:
	CCharacterFollower*	m_pCharacter;

	CRenderObject*		m_pHorseRenderObj;
	CHorseRenderHandler* m_pHorseRenderHandler;
	CWeaponRenderHandler* m_pWeaponRenderHandler;
	COffWeaponRenderHandler* m_pOffWeaponRenderHandler;
	CRenderObject*		m_pMainHandRenderObj;
	CRenderObject*		m_pOffHandRenderObj;
	CRenderObject*		m_pAdditionRenderObj;

	string						m_strHorseEffect;
	string						m_strMainHandEffect;
	string						m_strOffHandEffect;
	string						m_strAdditionEffect;
	uint32						m_uHorseEffect;
	uint32						m_uMainHandEffect;
	uint32						m_uOffHandEffect;
	uint32						m_uAdditionEffect;
	// 变形武器
	EWeaponPlayType				m_eWeaponPlayType;
	EWeaponPlayType				m_eOffWeaponPlayType;
	bool						m_bWeaponIsOpen;
	CWeaponChangeAniTick*		m_pWeaponChangeAniTick;
	CWeaponResumeAniTick*		m_pWeaponResumeAniTick;
};

class CWeaponChangeAniTick : public CTick
{
public:
	CWeaponChangeAniTick(CRenderLinkMgr* pLinkMgr) : m_pLinkMgr(pLinkMgr) {}
	~CWeaponChangeAniTick() { UnRegisterTick(); m_pLinkMgr = NULL; }

	void RegisterTick()		{ UnRegisterTick(); CAppClient::Inst()->RegisterTick(this, 1000); }
	void UnRegisterTick()	{ CAppClient::Inst()->UnRegisterTick(this); }
	void OnTick()			{ m_pLinkMgr->DoWeaponAnimation(1); UnRegisterTick(); }

private:
	CRenderLinkMgr*		m_pLinkMgr;

};

class CWeaponResumeAniTick : public CTick
{
public:
	CWeaponResumeAniTick(CRenderLinkMgr* pLinkMgr) : m_pLinkMgr(pLinkMgr) {}
	~CWeaponResumeAniTick() { UnRegisterTick(); m_pLinkMgr = NULL; }

	void RegisterTick()		{ UnRegisterTick(); CAppClient::Inst()->RegisterTick(this, 20000); }
	void UnRegisterTick()	{ CAppClient::Inst()->UnRegisterTick(this); }
	void OnTick()			{ m_pLinkMgr->DoWeaponAnimation(0); UnRegisterTick(); }

private:
	CRenderLinkMgr*		m_pLinkMgr;
};

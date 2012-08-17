#pragma once
//------------------------------------------------------------------------------
/**
@class CFxSoundPlayer

特效音效播放回调基类.
用于逻辑控制播放不同音效.

使用方法: pRenderObj->SetNoneActEffectUnitHandler(index, player, player->GetUnitName());
(C) 2010 ThreeOGCMan
*/
#include "CEffectUnitHandler.h"
#include "ActorDef.h"

class CActorFollower;

class CFxSoundPlayer : public CEffectUnitHandler
{
public:
	CFxSoundPlayer();

	bool RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS );
	/// 重置, 以便播放新的声音
	void Reset();

	/// 返回要播放的cue名称
	virtual string GetCueName() const = 0;
	/// 返回音效轨道的自定义名称
	const TCHAR* GetUnitName() const;

protected:
	bool	m_bHasPlayed;
	string	m_strUnitName;
};

//------------------------------------------------------------------------------
inline void
CFxSoundPlayer::Reset()
{
	m_bHasPlayed = false;
}

//------------------------------------------------------------------------------
inline const TCHAR*
CFxSoundPlayer::GetUnitName() const
{
	return m_strUnitName.c_str();
}

/// 挥动音效
class CWhiskSoundPlayer : public CFxSoundPlayer
{
public:
	CWhiskSoundPlayer(uint32 uEntityID);
	void SetActionState(EActionState eActionState)		{m_eActionState = eActionState;}

private:
	string GetCueName() const;
	uint32				m_uEntityID;
	EActionState		m_eActionState;
};

/// 打击音效
class CHitSoundPlayer : public CFxSoundPlayer
{
public:
	CHitSoundPlayer(uint32 uEntityID);
	void SetActionState(EActionState eActionState)	{m_eActionState = eActionState;}

private:
	string GetCueName() const;
	uint32				m_uEntityID;
	EActionState		m_eActionState;
};

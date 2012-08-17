#pragma once
#include "CRenderable.h"
#include "EffectBase.h"
namespace sqr
{

class IEffectUnitHandler;

class IEffect 
	: public CRenderable
	, public CEffectMallocObject
{
public:
	virtual LPCSTR		GetEffectName() = 0;
	virtual LPCSTR		GetFullEffectName() = 0;
	virtual int			GetAttachAniNum() = 0;
	virtual LPCSTR		GetAttachAniName( int Num ) = 0;
	virtual LPCSTR		GetUserDesc() = 0;
	virtual bool		IsFrameSynch() = 0;
	virtual int			GetTimeRange() = 0;
	virtual bool		IsFollowBone(CRenderable* pParentObj) = 0;
	virtual void		OnLinked( CRenderable* pParentObj )	= 0;
	virtual void		OnUnlinked( void )	= 0;
	virtual void		SetRenderCallback( IEffectUnitHandler* pUnitHandler, const char* szUnitName ) = 0;
	virtual void		SetRenderCallback( IEffectUnitHandler* pUnitHandler, EBaseEffectUnit EUnit ) = 0;
	virtual bool		UpdateEfx(uint32 uCurTime, bool bReset) = 0;
};
}
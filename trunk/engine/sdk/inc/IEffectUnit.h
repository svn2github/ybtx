#pragma once

#include "EffectBase.h"
#include "IEffectUnitProp.h"

namespace sqr
{

class IEffectUnit;

class IEffectUnitHandler 
	: public GraphicRes
	, public CEffectMallocObject
{
public:
	virtual bool RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS ) = 0;
};

class IEffectUnit 
	: public CRenderable
	, public CEffectMallocObject
{
public:
	virtual IEffectUnitProp*	GetProp() = 0;
	virtual bool				SetProp( IEffectUnitProp* pEffectUnitProp ) = 0;
	virtual void				SetStartTime( uint32 STime,uint32 DTime ) = 0;
	virtual void				ResetTime( uint32 STime,uint32 DTime ) = 0;
	//virtual uint32				GetStartTime() = 0;
	virtual const CMatrix&		GetMatrix() = 0;
	virtual void				GetDesc( char * str )	= 0;
	virtual void				SetRenderCallback( IEffectUnitHandler* pUnitHandler ) = 0;
	virtual CRenderable*		GetParent() = 0;
	virtual void				RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS) = 0;
	virtual void				SetBindID( int16 SkeletalID, uint8 SocketID )	= 0;
	virtual int16				GetBindSkeletalId(void) = 0;
	virtual uint8				GetBindSocketId(void)	= 0;
	//virtual void				ChangeDestXYZ( float x, float y, float z ) = 0;
	virtual EBaseEffectUnit		GetEffectUniType(void)		= 0;
	virtual EMap<float,CVector3f>& GetDestMap(void)			= 0;
	virtual float				GetUnitPercent(void)		= 0;
	virtual bool				IsInRenderTime(void)		= 0;
	virtual void				SetRotate( float fY )		= 0;
	virtual void				SetDistortTime(uint32 dTime)	= 0;
	virtual void				OnLinked( CRenderable* pParentObj ) = 0;
	virtual void				OnUnlinked( void ) = 0;
	virtual const AniResult		Render( const CMatrix& matWorld, uint32 uCurTime,  RenderObjStyle* pRORS ) = 0;
};
}
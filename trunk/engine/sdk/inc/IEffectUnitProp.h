#pragma once
#include "CRefObject.h"
#include "CEffectMallocObject.h"
#include "EffectBase.h"

namespace sqr
{

class IEffectRead : public CEffectMallocObject
{
public:
	virtual void read( void* pBuf, size_t uSize ) = 0;
};

class IEffectWrite : public CEffectMallocObject
{
public:
	virtual void write( void* pBuf, size_t uSize ) = 0;
};

class IEffectUnitProp 
	: public CRefObject
	, public CEffectMallocObject
{
public:
	virtual uint32			GetID() = 0;
	virtual const char*		GetEffectFileName() = 0;
	virtual const char*		GetEffectClassName() = 0;
	virtual const char*		GetCustomName() = 0;
	virtual void			SetCustomName( const char* str ) = 0;

	virtual eUpdatePosType	GetUpdatePosType() = 0;
	virtual const char*		GetAttachName() = 0;
	virtual bool			IsFitAniName( const char* AniName ) = 0;
	virtual void			SetEffectLod(float Lod)	= 0;
	virtual float			GetEffectLod(void) = 0;

	virtual uint32			GetTimeRang() = 0;
	virtual void			SetTimeRang( uint32 TimeRang ) = 0;

	virtual int32			LoadBin( IEffectRead& File ) = 0;
	virtual int32			SaveBin( IEffectWrite& File ) = 0;
	virtual int32			LoadFullRes() = 0;

	virtual const GUID		GetGUID() = 0;
	virtual const char*		GetClassDescription() = 0;
	virtual EBaseEffectUnit	GetUintType(void)	  = 0;

	virtual const EMap<float,CVector3f>&	GetOffset()		= 0;
	virtual const EMap<float,ITexture*>&	GetTexture()	= 0;
	virtual const EMap<float,CColor>&	GetColor()		= 0;
	virtual const EMap<float,BYTE>&		GetScale()		= 0;

	virtual bool GetMaxSize(float& fValue) { return false;}
	virtual void _FreeRes(void) = 0;
	virtual void _Reclaim(void) = 0;
};

}
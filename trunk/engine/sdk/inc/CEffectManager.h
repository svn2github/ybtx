#pragma once

#include "EffectBase.h"
#include "CEffect.h"
#include "CGraphic.h"
#include "IEffectClassDesc.h"
#include "IEffectManager.h"
#include "CBufFile.h"
#include "PkgStream.h"


class CEffectFile : public IEffectRead
{
	CBufFile& m_BufFile;

public:
	CEffectFile( CBufFile& BufFile ) : m_BufFile( BufFile ){}
	void read( void* pBuf, size_t uSize ){ m_BufFile.read( pBuf, uSize ); }
	void Read( void* pBuf, size_t uSize ){ read(pBuf,uSize); }
};


class CEffectManager : public IEffectManager
{
	struct CLASS_DESC
	{
		IEffectClassDesc*						m_EffectDesc;
	};

	struct EffectGroupRecycle
	{
		IEffectGroup*							m_pEffectGroup;
		uint32									m_DeleteTime;
	};

	struct GUIDComp
	{
	public:
		bool operator()(const GUID& _Left, const GUID& _Right)const
		{
			return memcmp(&_Left,&_Right,sizeof(_Left))<0;
		}
	};
	

protected:

	EMap< EString, EffectGroupRecycle >			m_Recycle;

	typedef EMap< GUID, CLASS_DESC,GUIDComp >	EffectClassDescMap_t;
	EMap< EString, IEffectGroup* >				m_pEffectGroups;
public:
	EffectClassDescMap_t					m_EffectClassDesc;

protected:

	virtual IEffectGroup*		CreateEffectGroupFun(const char* szFileName);
	void						Render( uint32 CurTime );
public:
	CEffectManager(void);
	virtual ~CEffectManager(void);

	void					DeleteEffectGroup( IEffectGroup* pGroup );

	virtual IEffectUnitProp*		CreateEffectUnitProp( const GUID Guid, int Num );
	IEffectUnit*			CreateEffectUnit( IEffectUnitProp* pEffectProp );

	virtual bool			InitClassDesc( void );
	virtual HRESULT			CreateEffectGroup( const char* szFileName, IEffectGroup** pEffectGroup );
	virtual HRESULT			CreateEffect( IEffectGroup* pEffectGroup, const char* szEffectName, IEffect** pEffect );
	virtual void			Update( uint32 CurTime );
	virtual void			ClearEffectDesc( void );
			void			ClearRecycle();
};
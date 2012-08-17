#pragma once
#include "CEffectUnit.h"

namespace sqr
{
	class IEffectGroup;
}


class EFFECT_API CCombinationProp : public CEffectUnitProp
{
protected:
	
	int32					m_nEffectIndex;
	EString					m_strEffectFileName;
	IEffectGroup           *m_pEffectGroup;

	void					_UpdateParams(void);

	friend class CCombination;

public:
	CCombinationProp( uint32 ID, IEffectClassDesc* pEffectClassDesc );
	~CCombinationProp();
	bool					IsValid();

	virtual int32			LoadBin( IEffectRead& File );
	virtual int32			SaveBin( IEffectWrite& File );
	EBaseEffectUnit			GetUintType(void) { return eEfxCombination; }
};

class EFFECT_API CCombination : public CEffectUnit//组合特效
{
	DYNAMIC_DECLARE_EFFECTUNITBASE( CCombination )

	private:
	 CRenderable*		m_pEffectRenderObj;
	 EString		    m_strLastName;

	public:
	CCombination( IEffectUnitProp* pEffectUnitProp );
	virtual ~CCombination(void);

	virtual void		SetVisible( bool isVisible );
	virtual void		SetStartTime( uint32 STime,uint32 DTime );
	virtual void		ResetTime( uint32 STime,uint32 DTime );
	virtual void		OnUnlinked( void );
	virtual bool		SetProp( IEffectUnitProp* pEffectUnitProp );
	const EFFECT_RESULT RenderEx( const CMatrix& matWorld,  RenderObjStyle* pRORS = NULL );	
};

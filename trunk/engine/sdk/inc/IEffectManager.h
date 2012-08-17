#pragma once
#include "GraphicBase.h"
#include "EffectBase.h"
#include "IEffectGroup.h"
#include "IEffect.h"
#include "TSingleton.h"
namespace sqr
{
	class IEffectManager : public Singleton<IEffectManager>
	{
	public:
		virtual					~IEffectManager(void) {};
		virtual bool			InitClassDesc( void ) = 0;
		virtual HRESULT			CreateEffectGroup( const char* szFileName, IEffectGroup** pEffectGroup ) = 0;
		virtual HRESULT			CreateEffect( IEffectGroup* pEffectGroup, const char* szEffectName, IEffect** pEffect ) = 0;
		virtual void			Update( uint32 CurTime ) = 0;
		virtual void			ClearEffectDesc( void ) = 0;
	};
}
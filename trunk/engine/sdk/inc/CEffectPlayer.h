#pragma once
#include "CSceneMallocObject.h"
#include "TIDPtrMap.h"
#include "IEffect.h"
#include "IEffectGroup.h"
#include "CAsynFunParamsMgr.h"
#include "CDynamicObject.h"

namespace sqr
{
	struct EffectNode
	{
		EffectNode();
		~EffectNode();

		IEffect*		pEffect;
		uint32			iTime;
		uint32			iIndex;

		union
		{
			void*		_VoidPtr;
			uint32		_Uint32;
			int32		_Int32;
			char		_Char;
		};
	};

	struct FxCreateInfo : public CSceneMallocObject
	{
		FxCreateInfo(IEffectGroup* pGrp, const SString& str);

		IEffectGroup*	pEffectGroup;
		SString			szFxName;
	};

	class CEffectPlayer : public virtual CDynamicObject
	{
	public:
		static const uint32 PLAY_LOOP;
		static const uint32 PLAY_ONCE;
		CEffectPlayer(void);
		~CEffectPlayer(void);
		
		uint32		AddEffect( const char* szFxFile, const char* szFx, uint32 nPlayTime, IModelHandler * pHandler );
		uint32		AddEffectPtr( IEffect* pEfx , uint32 nPlayTime );

		void		SetEffectUnitHandler( uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName );
		void		SetEffectUnitHandler( uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName,uint32 eEffectUnitType);

		IEffect*	GetEffect(uint32 uID);
		bool		DelEffect(uint32 uID);
		void		ClearAllEfx(void);


	protected:
		bool		IsFull(void);		
		void		ClearSyncEfx(void);
		void		AyncFun(void);
		virtual bool _RegisterEffect( EffectNode* pEfxNode );
		virtual bool _LoadedEffect( EffectNode* pEfxNode );

		typedef SMap<uint32,FxCreateInfo>	FXAyncPool;
		typedef SSet<EffectNode*>			FXDelPool;
		 TIDPtrMap< EffectNode > m_pEfxMap;
		 FXAyncPool				 m_fxPool;
		 FXDelPool				 m_fxDelPool;
		 CAsynFunParamsMgr		 m_AsynParams;
	};
}

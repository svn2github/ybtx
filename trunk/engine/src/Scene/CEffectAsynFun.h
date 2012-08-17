#pragma once
#include "CAsynFunParamsMgr.h"
#include "GraphicBase.h"

namespace sqr
{
	class CEffectPlayer;
	class IEffectUnitHandler;

	class SetEffectHandlerName : public FunCommand
	{
	public:
		SetEffectHandlerName( CEffectPlayer* pEfxPlayer,uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName );
		~SetEffectHandlerName();
		DWORD	GetType(void) { return stGetType(); }	
		bool	Compare(const void* param) { return Index == *(const uint32*)param; }

		static	DWORD stGetType(void) { return 'EfHN'; }
	protected:
		uint32				Index;
		CEffectPlayer*		m_pEfxPlayer;
		IEffectUnitHandler* pHandler;
		SString				Name;
	protected:
		virtual bool Do(void);
	};

	class SetEffectHandlerType : public SetEffectHandlerName
	{
	public:
		SetEffectHandlerType( CEffectPlayer* pEfxPlayer,uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName,uint32 eEffectUnitType );
		~SetEffectHandlerType();
		DWORD	GetType(void) { return stGetType(); }

		static	DWORD stGetType(void) { return 'EfHT'; }
	protected:
		uint32 nType;
	protected:
		virtual bool Do(void);
	};

}
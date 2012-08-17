#pragma once
#include "TSingleton.h"
#include "CDynamicObject.h"
#include "CSceneMallocObject.h"

namespace sqr
{
	class IEffectGroup;

	class CEffectLoader 
		: public virtual CDynamicObject
		, public Singleton<CEffectLoader>
		, public CSceneMallocObject
	{
	public:
		CEffectLoader();
		~CEffectLoader();
		static CEffectLoader*		Inst();

		void	AddEftGroup( const char* szFileName );
		void	FreeEftGroup( const char* szFileName );
		void	Clear();
		void	SetGroupHandle(HWND hwnd);
		
	private:
		static CEffectLoader*&		IntInst();

		typedef SMap<SString, IEffectGroup*>  EGroupMap;
		EGroupMap	m_MEffectGroup;
		HWND		m_hWnd;
	};

}
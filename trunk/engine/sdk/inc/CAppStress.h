#pragma once

#include "TExpSafeApp.h"
#include "TAppWithConsole.h"
#include "CTraitsStress.h"
#include <iostream>

namespace sqr
{
	class IConnMgrStressHandler;
	class CConnMgrStress;

	class CAppStress
		: public TExpSafeApp<CTraitsStress>
		, public TAppWithConsole<CTraitsStress>
	{
	public:
		static void Init();
		static void Unit();

		uint32 GetGlobalTime()const;

		void Quit();

	private:
		void OnBeginMainCycleDerived();
		void OnMainFrameEndedDerived();
#ifndef _WIN32
		static void SigHandler(int signum);
#else
		static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);
#endif
	
		void OnCanceled();

		typedef TExpSafeApp<CTraitsStress> ParentApp_t;
		friend class TApp<CTraitsStress>;
		friend class CConnMgrStress;
		friend class CConnStress;

		CAppStress();
		~CAppStress();
	};
}

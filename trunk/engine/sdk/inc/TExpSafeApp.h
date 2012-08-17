#pragma once 
#include "TApp.h"

namespace sqr
{

	template<typename Traits>
	class TExpSafeApp
		: public TApp<Traits>
	{
		typedef TApp<Traits>	ParentApp_t;

		typedef typename Traits::App_t					ImpApp_t;
		typedef typename Traits::AppHandler_t			ImpAppHandler_t;
		
	public:
		static void TerminateHandler();

#ifdef _WIN32
		static void InvalidParameterHandler(const wchar_t* expression,const wchar_t* function,
			const wchar_t* file, unsigned int line, uintptr_t pReserved);
		static void PurecallHandler(void);
#endif

	protected:
		TExpSafeApp();
		~TExpSafeApp();

	private:
		typedef void (*Terminate_Fun)();
		Terminate_Fun	m_PreTerminateFun;
#ifdef _WIN32
		_invalid_parameter_handler	m_PreParamHandler;
		_purecall_handler			m_PrePurecallHandler;
		_se_translator_function		m_PreSeTransFunction;
#endif
	};
}

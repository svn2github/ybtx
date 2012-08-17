#pragma once
#include "TSingleton.h"
namespace sqr_tools
{
	class CTaskProcess
	{
	public:
		typedef set<wstring> ExtNameSet;

		CTaskProcess();
		~CTaskProcess();

		virtual void			Do(const wchar_t* szfilename)	= 0;
		virtual const string	GetName() const					= 0;
		virtual ExtNameSet		GetExtNames()					= 0;

	protected:
	};
}

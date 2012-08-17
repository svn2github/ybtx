#include "stdafx.h"
#include "CLRStringHelper.h"

namespace sqr_tools
{
	void CLRMarshalString( System::String^ s, std::string& os)
	{
		using namespace System::Runtime::InteropServices;
		const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		os = chars;
		Marshal::FreeHGlobal(IntPtr((void*)chars));
	}

	char*	CLRMarshalToCharPtr(System::String^ s)
	{
		using namespace System::Runtime::InteropServices;
		char* chars = (char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		return chars;
	}

	void CLRMakePathToText(const string& Src, string& Dst)
	{
		Dst = Src;
		size_t npos = Dst.rfind("\\");
		if( npos != -1 )
		{
			Dst = Dst.substr(npos+1, Dst.length());
			npos = Dst.find(".");
			if( npos != -1 )
				Dst = Dst.substr(0, npos);
		}
	}
}
#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

namespace sqr_tools
{
	void			CLRMarshalString( System::String^ s, std::string& os);
	char*			CLRMarshalToCharPtr(System::String^ s);
	void			CLRMakePathToText(const string& Src, string& Dst);
}
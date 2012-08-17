#pragma once

inline System::String^ UnmagStringToMagString(const char* lp)
{
	return System::Runtime::InteropServices::Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(lp)));
}

inline const char* MagStringToUnmagString(System::String^ str)
{
	return static_cast<const char*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(str).ToPointer());
}

inline System::Object^ UnknowToObject(void* unknow)
{
	System::IntPtr lp=static_cast<System::IntPtr>(unknow);
	return System::Runtime::InteropServices::Marshal::GetObjectForIUnknown(lp);
}

inline void ObjectToUnknow(System::Object^ obj,void* unknow)
{
	System::IntPtr lp=System::Runtime::InteropServices::Marshal::GetIUnknownForObject(obj);
	unknow=lp.ToPointer();
}
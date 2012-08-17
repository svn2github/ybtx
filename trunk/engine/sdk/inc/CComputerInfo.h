#pragma once
#include "GraphicBase.h"
#include "TSingleton.h"

namespace sqr
{
	class CComputerInfo 
		: public Singleton<CComputerInfo>
		, public CGraphicMallocObject
	{
	public:
		CComputerInfo()
		{
			m_OS = "";
		}
		string			m_OS;
		string			GetAudioCardInfo();
		string			GetCpuInfo();
		string			GetDirectInfo();
		string			GetHDInfo();
		string			GetOSInfo();
		string			GetMemInfo();
	};
}
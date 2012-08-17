#pragma once
#include "AutoDataHelper.h" 
#include "CXmlConfig.h"

namespace sqr_tools
{
	class CXMLDataSerializer
	{
	public:
		bool	FromXml(AutoDataHelper* pAutoData,TiXmlNode* Node);
		bool	ToXml(AutoDataHelper* pAutoData,TiXmlNode* Node);
	};
}
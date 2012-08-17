#pragma once
#include "tinyxml/tinyxml.h"
#include "SQRWnd.h"
#include "GUIBase.h"
#include "SQRGuiMallocObject.h"

namespace sqr
{
	// 功能：XMLhandler抽象类
	class GUI_API XMLHandler : public SQRGuiMallocObject
	{
	public:
		// 构造函数
		XMLHandler(void)			{};
		// 析构函数
		virtual ~XMLHandler(void)	{};

		// 解析相关
		virtual void ElementStart(const char* element, TiXmlElement* ElementPtr)	{};
		virtual void ElementEnd	 (const char* element)							{};
	};
	
}
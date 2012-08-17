#pragma once

#include "GUIXmlParser.h"
#include "CodeCvs.h"

namespace sqr
{
	// UI XML解析的抽象类
	class GUI_API StuffParser : public XMLParser
	{
	public:
		// 构造函数
		StuffParser(void);
		// 析构函数
		virtual ~StuffParser(void);
		// 抽象函数，解析XML文件 
		void ParseXMLFile(XMLHandler& handler, TiXmlDocument& doc);

	protected:
		// 初始化解析器模块的抽象函数
		bool InitilaliseImpl(void)	{ return true; }
		// 清空解析器
		void CleanupImpl(void)		{ return;	   }
		// 解析DOC
		bool ParXMLElement(XMLHandler* handler,TiXmlElement* ElementPtr);

		// 回调的handlers
		void StartElement(XMLHandler* handler, const char* element, TiXmlElement* attr);
		void EndElement(XMLHandler* handler, const char* element);

	};
}
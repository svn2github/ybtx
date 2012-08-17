#pragma once

#include "stdafx.h"
#include "GUIXmlHandler.h"

namespace sqr
{
	// UI XML解析的抽象类
	class GUI_API XMLParser
	{
	public:
		// 构造函数
		XMLParser(void);
		// 析构函数
		virtual ~XMLParser(void);
		// 初始化解析器模块
		bool Initialise(void);
		// 清空解析器
		void Cleanup(void);
		// 抽象函数，解析XML文件 
		virtual void ParseXMLFile(XMLHandler& handler, TiXmlDocument& doc) = 0;
		// 从解析单元获取标记的string
		const char* GetIdentifierString() const;

	protected:
		// 初始化解析器模块的抽象函数
		virtual bool InitilaliseImpl(void) = 0;
		// 清空解析器
		virtual void CleanupImpl(void) = 0;
		// 解析模块的字符串信息
		UIString m_idtfString;

	private:
		bool m_beInit;			// 解析模块被初始化了 =true.
	};
}
#pragma once
#include "ModuleDefs.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

class TiXmlDocument;
class TiXmlNode;
class TiXmlElement;

namespace sqr
{
	//警告：不要把这个类改成通用的Xml文件读写类，这个类的主要作用是"Config"，性能方面是很差的，要进行强大复杂的xml读写，请直接使用tinyxml

	class MODULE_API CXmlSiblingElemIter
	{
	public:
		CXmlSiblingElemIter();
		void SetCurNode(TiXmlNode* pNode, const char* szElemName);
		void MoveNext();
		bool IsEnd();
		const char* GetSiblingName() const;
		const char* Get(const char* szElemName)const;
	protected:
	private:
		string m_strSiblingName;
		TiXmlNode* m_pCurNode;
	};


	class MODULE_API CXmlConfig
	{
	public:
		CXmlConfig();
		CXmlConfig(const char* szRootElementName);
		CXmlConfig(const char* szRootElementName,istream& Stream);
		CXmlConfig(TiXmlElement* pElement);

		~CXmlConfig();

		bool LoadConfig(const char* szRootNodeName,const char* szFileName);
		void LoadConfig(const char* szRootNodeName,istream& Stream);
		bool LoadConfigFormString(const char* szRootNodeName, const char* szXml);
		void SaveConfig(const char* szFileName);
		TiXmlElement* GetRootNode();

		template<typename ValueType>
		ValueType DftGet(ValueType DefaultValue,const char* szElemName)const;
		template<typename ValueType>
		ValueType DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2)const;
		template<typename ValueType>
		ValueType DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szElemName3)const;

		template<typename ValueType>
		ValueType Get(const char* szElemName)const;
		template<typename ValueType>
		ValueType Get(const char* szElemName1,const char* szElemName2)const;
		template<typename ValueType>
		ValueType Get(const char* szElemName1,const char* szElemName2,const char* szElemName3)const;

		void GetIter(CXmlSiblingElemIter* it, const char* szElemName)const;
		void GetIter(CXmlSiblingElemIter* it,const char* szElemName,const char* szElemName2)const;

		void	  Set(const char* szParElemName,const char* szChildElemName, const char* szElemTexName);
		void	  Insert(const char* szParElemName,const char* szChildElemName);

		template<typename ValueType>
		ValueType DftAttribGet(ValueType DefaultValue,const char* szAttribName)const;
		template<typename ValueType>
		ValueType DftAttribGet(ValueType DefaultValue,const char* szElemName1,const char* szAttribName)const;
		template<typename ValueType>
		ValueType DftAttribGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szAttribName)const;

		template<typename ValueType>
		ValueType AttribGet(const char* szAttribName)const;
		template<typename ValueType>
		ValueType AttribGet(const char* szElemName1,const char* szAttribName)const;
		template<typename ValueType>
		ValueType AttribGet(const char* szElemName1,const char* szElemName2,const char* szAttribName)const;

		void	  AttribSet(const char* szParElemName, const char* szChildElemName);
		void	  AttribSet(const char* szParElemName, const char* szChildElemName, const char* szAttribValueName, const char* szAttribValueValue);
		void	  AttribSet(const char* szParElemName, const char* szChildElemName, const vector<string>& vecAttribValueName, const vector<string>& vecAttribValueValue);
		void	  AttribSet(const char* szParElemName1,const char* szParElemName2,  size_t parElemName2Index, const char* szChildElemName, const vector<string>& vecAttribValueName, const vector<string>& vecAttribValueValue);
		void	  AttribSet(const vector<const char*>& szParElemNames,  const char* szChildElemName, const vector<string>& vecAttribValueName, const vector<string>& vecAttribValueValue);
		
		//警告：不要把这个类改成通用的Xml文件读写类，这个类的主要作用是"Config"，性能方面是很差的，要进行强大复杂的xml读写，请直接使用tinyxml

	private:
		TiXmlElement*	m_pRoot;
		
		TiXmlElement* GetNode(const vector<const char*>& vecElemName,bool bGenErr)const;
		
		template<typename ValueType>
		ValueType GetNodeValue(const ValueType* pDefaultValue,TiXmlNode* pNode,const vector<const char*>& vecElemName)const;
		
		template<typename ValueType>
		ValueType GetNodeAttribValue(const ValueType* pDefaultValue,TiXmlNode* pNode,const char* szAttribName,const vector<const char*>& vecElemName)const;

		void GenElemNoContentErr(const vector<const char*>& vecElemName)const;
		void GenAttribNoContentErr(const char* szAttribName,const vector<const char*>& vecElemName)const;

		void Clear();
	};

	template<> MODULE_API const char* CXmlConfig::GetNodeValue<const char*>(const char*const* pDefaultValue,TiXmlNode* pNode,const vector<const char*>& vecElemName)const;
	template<> MODULE_API const char* CXmlConfig::GetNodeAttribValue<const char*>(const char*const* pDefaultValue,TiXmlNode* pNode,const char* szAttribName,const vector<const char*>& vecElemName)const;
	template<> MODULE_API TiXmlElement* CXmlConfig::Get<TiXmlElement*>(const char* szElemName)const;
	template<> MODULE_API TiXmlElement* CXmlConfig::Get<TiXmlElement*>(const char* szElemName1,const char* szElemName2)const;
	template<> MODULE_API TiXmlElement* CXmlConfig::Get<TiXmlElement*>(const char* szElemName1,const char* szElemName2,const char* szElemName3)const;

	
}

#ifdef _WIN32
#pragma warning (pop)
#endif

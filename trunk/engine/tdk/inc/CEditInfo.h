#pragma once
#include "tinyxml/tinyxml.h"

namespace sqr_tools
{
	class CEditFormatBase;

	class CEditInfo
	{
		friend class CEditFormatBase;

	public:
		CEditInfo(){}
		CEditInfo(CEditFormatBase* edit_format);
		virtual ~CEditInfo();

		void			RefersTo(CEditInfo* edit_info);
		const string&	GetDeadPath()	const;
		const string&	GetLivePath()	const;
		string			GetSavePath()	const;

	protected:
		TiXmlDocument*	getXmlDoc();

		/**
		* 在after_this节点(必须是最上层节点)之后插入"<node>text</node>"形式的节点,并返回被插入的节点指针
		*/
		TiXmlNode*		xmlInsertNodeTextAfter(TiXmlNode* after_this, const string& node_name, const string& node_text);

	private:
		/**
		* 继承这个方法去存私有编辑信息
		*/
		virtual void	saveCustomInfo() {};

	private:
		void			addReferenceParent(const string& ref_parent);
		void			addXmlDecl();
		void			setDeadPath(const string& dead_path);
		void			setLivePath(const string& live_path);
		void			writeXmlData(FILE* file);
		void			saveReference();
		void			saveEditList();

		typedef list<CEditInfo*>	EditInfoList;
		typedef set<CEditInfo*>		EditInfoSet;
		typedef set<string>			ReferenceSet;

		CEditFormatBase*	m_edit_format;
		TiXmlDocument		m_xml_doc;
		EditInfoList		m_edit_info_list;
		EditInfoSet			m_edit_info_set;	// 检测重复注册
		string				m_dead_path;		// tgt之前的绝对路径(不包含tgt)
		string				m_live_path;		// tgt之后的相对路径(比如charactor\tex\ar001.dds)
		ReferenceSet		m_ref_parent_set;	// 引用的其他文件列表

	public:
		static const string	REF_ROOT_NODE;
		static const string	REF_ELEMENT_NODE;
	};

}

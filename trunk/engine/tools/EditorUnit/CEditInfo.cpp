#include "stdafx.h"
#include "CEditInfo.h"
#include "BaseHelper.h"
#include "CEditFormatBase.h"

namespace sqr_tools
{
	const string CEditInfo::REF_ROOT_NODE		= "reference";
	const string CEditInfo::REF_ELEMENT_NODE	= "refer";

	CEditInfo::CEditInfo(CEditFormatBase* edit_format)
		: m_edit_format(edit_format)
	{
	}

	CEditInfo::~CEditInfo()
	{
	}

	void CEditInfo::RefersTo( CEditInfo* edit_info )
	{
		if ( !m_edit_info_set.insert(edit_info).second )
			return;

		edit_info->addReferenceParent(m_live_path);
		m_edit_info_list.push_back(edit_info);
	}

	void CEditInfo::saveEditList()
	{
		if ( m_edit_info_list.empty() )
			return;

		EditInfoList::iterator it = m_edit_info_list.begin();
		for ( ; it != m_edit_info_list.end(); ++it )
		{
			CEditInfo* cur_edit = (*it);
			cur_edit->m_edit_format->saveEditData(cur_edit->GetSavePath(), cur_edit);
			cur_edit->saveEditList();
		}
	}

	void CEditInfo::addXmlDecl()
	{
		if ( m_xml_doc.FirstChild() != 0 && m_xml_doc.FirstChild()->Type() != TiXmlNode::DECLARATION )
			MessageBox(0, "xml declaration added more than once", "CEditInfo::addXmlDecl() error", MB_OK);

		TiXmlDeclaration xml_decl;
		xml_decl.Parse("<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UTF8);
		m_xml_doc.InsertEndChild(xml_decl);
	}

	const string& CEditInfo::GetDeadPath() const
	{
		return m_dead_path;
	}

	void CEditInfo::setDeadPath(const string& dead_path)
	{
		m_dead_path = dead_path;
	}

	void CEditInfo::setLivePath(const string& live_path)
	{
		m_live_path = live_path;
	}

	void CEditInfo::saveReference()
	{
		if ( m_ref_parent_set.empty() )
			return;

		TiXmlNode* ref_root_node = m_xml_doc.FirstChild(REF_ROOT_NODE);
		if ( !ref_root_node )
			ref_root_node = m_xml_doc.InsertEndChild(TiXmlElement(REF_ROOT_NODE));

		// get existed reference info, used to check multiple reference. 
		ReferenceSet exist_ref_set;
		TiXmlElement* cur_ref_elem = ref_root_node->FirstChildElement(REF_ELEMENT_NODE);
		while ( cur_ref_elem != 0 )
		{
			exist_ref_set.insert(cur_ref_elem->GetText());
			cur_ref_elem = cur_ref_elem->NextSiblingElement();
		}

		// add reference info and check multiple reference.
		ReferenceSet::iterator ref_iter = m_ref_parent_set.begin();
		for ( ; ref_iter != m_ref_parent_set.end(); ++ref_iter )
		{
			string ref_parent = *ref_iter;
			
			if ( exist_ref_set.find(ref_parent) != exist_ref_set.end() )
				continue;

			TiXmlElement ref_element(REF_ELEMENT_NODE);
			TiXmlText	 ref_text(ref_parent);
			ref_element.InsertEndChild(ref_text);
			ref_root_node->InsertEndChild(ref_element);
		}
	}

	void CEditInfo::writeXmlData(FILE* file)
	{
		m_xml_doc.Clear();

		addXmlDecl();
		saveReference();

		saveCustomInfo();

		fprintf( file, "\n" );
		for ( const TiXmlNode* node=m_xml_doc.FirstChild(); node; node=node->NextSibling() )
		{
			node->Print( file, 0 );
			fprintf( file, "\n" );
		}
	}

	const string& CEditInfo::GetLivePath() const
	{
		return m_live_path;
	}

	string CEditInfo::GetSavePath() const
	{
		return m_dead_path;//+ CPathBrowser::SRC_FOLDER_NAME + "/" + m_live_path + "_s";
	}

	TiXmlNode* CEditInfo::xmlInsertNodeTextAfter(TiXmlNode* after_this, const string& node_name, const string& node_text)
	{
		TiXmlNode* node_exist = m_xml_doc.FirstChildElement(node_name);
		if ( node_exist )
			return node_exist;

		TiXmlElement xml_node(node_name);
		TiXmlText	 xml_node_txt(node_text);
		xml_node.InsertEndChild(xml_node_txt);
		node_exist = m_xml_doc.InsertAfterChild(after_this, xml_node);

		return node_exist;
	}

	TiXmlDocument* CEditInfo::getXmlDoc()
	{
		return &m_xml_doc;
	}

	void CEditInfo::addReferenceParent(const string& ref_parent)
	{
		m_ref_parent_set.insert(ref_parent);
	}
}

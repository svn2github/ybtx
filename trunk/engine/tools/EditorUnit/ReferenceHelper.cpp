#include "stdafx.h"
#include "tinyxml/tinyxml.h"
#include "PathBrowser/pathbrowser.h"

namespace sqr_tools
{
	void AddReference(const string& refer_from, const string& refer_to)
	{
		string res_from = refer_from;
		string res_to	= refer_to;
		string res_src;
		
		if ( !CPathBrowser::GetResFileName(res_src, res_from) )
		{
			LogErr(refer_from + " not found in search path list.\n");
			return;
		}

		res_src = res_src.substr(0, res_src.rfind(CPathBrowser::TGT_FOLDER_NAME)) + CPathBrowser::SRC_FOLDER_NAME + "\\";
		res_to += "_s";
		string save_path = res_src + res_to;

		TiXmlDocument		xml_doc;
		TiXmlDeclaration	xml_decl;
		TiXmlElement		xml_refer("refer_from");

		xml_decl.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UTF8 );
		xml_doc.InsertEndChild(xml_decl);
		xml_doc.SaveFile(save_path);
	}

}

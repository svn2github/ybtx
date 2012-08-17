#include "stdafx.h"
#include "GUIPicSaver.h"
#include "tinyxml/tinyxml.h"
#include "PkgStream.h"
#include "StringHelper.h"
#include "CodeCvs.h"
#include "CSvnControl.h"
#include "CPathMgr.h"

string save_root = "root";
string save_texture = "texture";
string save_res[eRes_T_count] = { "res_gui", "res_style", "res_flash", "res_emt", "res_texft","res_pro" };

GUIPicSaver::GUIPicSaver()
: m_bLoaded(false)
{}

GUIPicSaver::~GUIPicSaver()
{}

void GUIPicSaver::AddXmlResData(Res_type rt, string& str)
{
	StrSetMap::iterator itr = m_PicRes.find(rt);
	if ( itr == m_PicRes.end() )
	{
		StrSet strSet;
		m_PicRes.insert(make_pair(rt, strSet));
		itr = m_PicRes.find(rt);
	}

	StrSet& strset = itr->second;
	strset.insert(str);
}

void GUIPicSaver::DelXmlResData(Res_type rt, string& str)
{
	StrSetMap::iterator itr = m_PicRes.find(rt);
	if ( itr == m_PicRes.end() )
	{
		StrSet strSet;
		m_PicRes.insert(make_pair(rt, strSet));
		return;
	}

	StrSet& strset = itr->second;
	StrSet::iterator stritr = strset.find(str);
	if (stritr != strset.end())
		strset.erase(stritr);
}

void GUIPicSaver::ClearXmlResData(Res_type rt)
{
	StrSetMap::iterator itr = m_PicRes.find(rt);
	if ( itr == m_PicRes.end() )
	{
		StrSet strSet;
		m_PicRes.insert(make_pair(rt, strSet));
		return;
	}

	StrSet& strset = itr->second;
	strset.clear();
}

bool GUIPicSaver::Load( wstring& path )
{
	return false;
	if (!m_bLoaded)
	{
		m_ResPath = path;
		// load the res data
		wstring pa = L"D:\\ybtx\\designer\\cfg\\gui\\" + path;
		TiXmlDocument XmlDoc;
		ifstream iXML( pa.c_str() );
		if (!iXML)
			return false;
		iXML>>XmlDoc;
		iXML.close();

		TiXmlNode* ResRoot = XmlDoc.FirstChild( save_root.c_str() );
		
		Res_type res_tp = eRes_T_GUI;
		for( TiXmlNode* pNode = ResRoot->FirstChild( save_res[res_tp].c_str() );res_tp < eRes_T_count; pNode = ResRoot->FirstChild( save_res[res_tp].c_str() ) )
		{
			if (!pNode)
			{
				res_tp = (Res_type)(res_tp + 1);
				continue;
			}

			for (TiXmlNode* pTexNode = pNode->FirstChild(save_texture.c_str());pTexNode;pTexNode = pTexNode->NextSibling(save_texture.c_str()))
			{
				TiXmlElement* pElement = pTexNode->ToElement();
				if (!pElement)
					continue;

				// TEXTURE NAME
				string temp_texname = pElement->GetText();
				StrSetMap::iterator itr = m_PicRes.find(res_tp);
				if ( itr == m_PicRes.end() )
				{
					StrSet strSet;
					m_PicRes.insert(make_pair(res_tp, strSet));
					itr = m_PicRes.find(res_tp);
				}

				replace( temp_texname.begin(), temp_texname.end(), '/', '\\' );
				StrSet& strset = itr->second;
				strset.insert(temp_texname);
			}

			res_tp = (Res_type)(res_tp + 1);
			if (res_tp == eRes_T_count)
				break;
		}
		m_bLoaded = true;
		return true;
	}
	return false;
}

void GUIPicSaver::Save()
{
	// 这个保存啊，直接保存现有的数据
	return;
	if (0)
	{
		ProcessSvnFile();

		// save all the file that del and can not find
		string save_path = utf16_to_gbk(m_ResPath);
		save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) + "delfile.xml";

		TiXmlDocument		ResDoc;
		TiXmlElement		ResRoot(save_root.c_str());

		StrSet::iterator itr = m_DelFile.begin();
		StrSet::iterator eitr = m_DelFile.end();
		while( itr != eitr )
		{
			TiXmlElement		ResTex(save_texture.c_str());
			TiXmlText			ResTexVal(*itr);
			ResTex.InsertEndChild(ResTexVal);
			ResRoot.InsertEndChild(ResTex);
			++itr;
		}

		TiXmlElement		ResTex(save_texture.c_str());
		TiXmlText			ResTexVal("****************************************************");
		ResTex.InsertEndChild(ResTexVal);
		ResRoot.InsertEndChild(ResTex);

		itr = m_DelFileFailed.begin();
		eitr = m_DelFileFailed.end();
		while( itr != eitr )
		{
			TiXmlElement		ResTex(save_texture.c_str());
			TiXmlText			ResTexVal(*itr);
			ResTex.InsertEndChild(ResTexVal);
			ResRoot.InsertEndChild(ResTex);
			++itr;
		}

		ResDoc.InsertEndChild(ResRoot);
		ResDoc.SaveFile(save_path);
		return;
	}
	string save_path = utf16_to_gbk(m_ResPath);
	save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) + save_path;

	TiXmlDocument		ResDoc;
	TiXmlElement		ResRoot(save_root.c_str());

	string texpath = utf16_to_gbk(CPkgFile::FindPathInclude(L"resbin"));
	string texnewpath = texpath + "guitex\\res_tex";

	StrSetMap::iterator it = m_PicRes.begin();
	StrSetMap::iterator eit = m_PicRes.end();
	while( it != eit )
	{
		Res_type res_type = it->first;
		TiXmlElement		ResTp(save_res[res_type].c_str());

		StrSet& str_set = it->second;
		StrSet::iterator itr = str_set.begin();
		StrSet::iterator eitr = str_set.end();

		while( itr != eitr )
		{
			TiXmlElement		ResTex(save_texture.c_str());
			TiXmlText			ResTexVal(*itr);
			ResTex.InsertEndChild(ResTexVal);
			ResTp.InsertEndChild(ResTex);

			// 保存所有文件
			string str_tex_save = *itr;
			replace( str_tex_save.begin(), str_tex_save.end(), '/', '\\' );
			tolower( str_tex_save );
			m_AllTex.insert(str_tex_save);
			// 完成拷贝工作
			/*string nm = *itr;
			string src = texpath + nm;
			string sav = nm.substr(6, nm.length() - 6);
			string des = texnewpath + sav;

			CString CSrc = gbk_to_utf16(src.c_str()).c_str();
			CString CDes = gbk_to_utf16(des.c_str()).c_str();

			CopyFile(CSrc, CDes, TRUE);*/
			++itr;
		}
		++it;
		ResRoot.InsertEndChild(ResTp);
	}

	ResDoc.InsertEndChild(ResRoot);
	ResDoc.SaveFile(save_path);
}

FTW_RESULT ProcessTexFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* pContext )
{
	if( eFlag == _FTW_DIR && !_wcsicmp( szFileName + wcslen( szFileName ) - 4, L".svn" ) )
		return _FTW_IGNORE;

	GUIPicSaver* pSaver = (GUIPicSaver*)pContext;
	string sOldLocale=setlocale(LC_CTYPE,NULL);

	wstring filename = szFileName;
	string strpath = "D:\\ybtx\\artist\\resbin\\guitex";
	string str_file_name = utf16_to_gbk(filename).c_str();
	str_file_name = str_file_name.substr(strpath.length() + 1, str_file_name.length() - strpath.length() - 1);
	string str_fname = "guitex\\" + str_file_name;
	replace( str_fname.begin(), str_fname.end(), '/', '\\' );
	tolower( str_fname );
	if( eFlag == _FTW_DIR )
	{
		// 处理文件夹
		if (pSaver->GetAllTex().find(str_fname) != pSaver->GetAllTex().end())
			return _FTW_IGNORE;
		return _FTW_CONTINUNE;
	}
	else
	{
		// 处理文件
		if (pSaver->GetAllTex().find(str_fname) == pSaver->GetAllTex().end())	// 删除文件
		{
			pSaver->DeleteSvnFile(str_file_name);
		}
		return _FTW_CONTINUNE;
	}
	setlocale(LC_CTYPE, sOldLocale.c_str());
}

void GUIPicSaver::ProcessSvnFile()
{
	char* pSvn_Path = "D:\\ybtx\\artist\\resbin\\guitex";
	string strSvn_Path = pSvn_Path;

	FileTreeWalk( gbk_to_utf16(strSvn_Path).c_str(), ProcessTexFile, this );
	DeleteFile();
}

void GUIPicSaver::DeleteSvnFile(string file)
{
	m_DelFile.insert(file);
}

void GUIPicSaver::DeleteFile()
{
	char* pSvn_Path = "D:\\ybtx\\artist\\resbin\\guitex";
	CSvnControl SvnCtrl(pSvn_Path);

	string user = "chengxu";
	string pwd = "123456";

	SvnCtrl.setUserName(user);
	SvnCtrl.setPassWord(pwd);

	StrSet::iterator itr = m_DelFile.begin();
	while(itr != m_DelFile.end())
	{
		string file = *itr;
		if (file == "tex/ybtx/gui/2ndEdition/Icon/equip_piece.tex")
		{
			int ii = 0;
			++ii;
		}

		if(!(SvnCtrl.svn(eSVNC_DELETE,file,"删除文件")))
			m_DelFileFailed.insert(file);	
		++itr;
	}
}

set<string>& GUIPicSaver::GetAllTex()
{
	return m_AllTex;
}


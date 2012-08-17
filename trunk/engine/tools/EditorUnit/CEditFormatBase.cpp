#include "stdafx.h"
#include "CEditFormatBase.h"
#include "tinyxml/tinyxml.h"
#include "CPathMgr.h"
#include "StringHelper.h"
#include "CodeCvs.h"
#include "BaseHelper.h"
#include "CEditInfo.h"
#include <string>
#include "CEditPieceGroup.h"
#include "CModCoder.h"
using namespace std;


namespace sqr_tools
{
	CEditFormatBase::CEditFormatBase()
		: CFileFormatMgr(0, 0)
	{
	}

	CEditFormatBase::~CEditFormatBase()
	{
	}

	bool CEditFormatBase::saveGameData(const string& file_name, CEditInfo* handle)
	{
		string save_name = _nameFilter(file_name); 
		PathGroup path_group;
		pathFilter(save_name, path_group);
		
		return _saveGameData(path_group.game_path, handle);
	}

	bool CEditFormatBase::saveEditData(const string& file_name, CEditInfo* handle)
	{
		string save_name = _nameFilter(file_name);
		PathGroup path_group;
		pathFilter(save_name, path_group);

		CEditInfo* edit_onfo = (CEditInfo*)handle;
		edit_onfo->setDeadPath(path_group.dead_path);
		edit_onfo->setLivePath(path_group.live_path);
		_registerRefernceInfo(handle);

		createSaveDirectory(path_group.edit_path);

		FILE* file = _wfopen( gbk_to_utf16(path_group.edit_path).c_str(), L"wb" );
		if( !file )
			return false;

		// write binary data
		WriteDataInf wdi(file);
		wdi.BeginWrite(m_FileType, m_FileVer, CDataChunk::DCT_DEPEND);
		_AutoEncode(wdi, handle);	
		wdi.EndWrite();

		// write xml data
		edit_onfo->writeXmlData(file);
		edit_onfo->saveEditList();

		fclose( file );
		file = NULL;
		return true;
	}

	bool CEditFormatBase::Save( const string& file_name, OBJ_HANDLE handle )
	{
		CModFormat ModFormat;
		ModFormat.RegisterCoder(new CPieceGroupCoder);
		ModFormat.Save(file_name,handle);
		//EditStruct edit_struct = _getEditStruct(handle);

		//if ( !saveGameData(file_name, edit_struct.edit_info) )
		//	return false;
		//
		//if ( !saveEditData(file_name, edit_struct.edit_info) )
		//	return false;

		return true;
	}

	bool CEditFormatBase::isOpenResbin(const string& real_path, const string& cur_path)
	{
		return true;
	}

	bool CEditFormatBase::Load( const string& file_name,OBJ_HANDLE handle )
	{

		CModFormat ModFormat;
		ModFormat.RegisterCoder(new CPieceGroupCoder);
		ModFormat.Load(file_name,handle);
		//string save_name = _nameFilter(file_name);
		//save_name = CPathBrowser::GetResRealPath(save_name);
		//
		//PathGroup path_group;
		//pathFilter(save_name, path_group);

		//EditStruct edit_struct = _getEditStruct(handle);
		//edit_struct.edit_info->setDeadPath(path_group.dead_path);
		//edit_struct.edit_info->setLivePath(path_group.live_path);
		//
		//if ( isOpenResbin(save_name, path_group.live_path) )
		//{
		//}

		//// ---- read game data from tgt path

		//_loadGameData(path_group.game_path, edit_struct.edit_info);

		//// ---- read edit data from src path

		//CodeFileInf *pFInf = new CodeFileInf(this,handle);
		//SQR_TRY	
		//{
		//	if ( CPkgFile::ReadWholeFile( path_group.edit_path.c_str(), pFInf, _OnReadAsyn ) < 0 )
		//	{	
		//		saveEditData(path_group.edit_path, edit_struct.edit_info);
		//		if (CPkgFile::ReadWholeFile( path_group.edit_path.c_str(), pFInf, _OnReadAsyn ) < 0)
		//		{
		//			LogErr("打开编辑文件失败", path_group.edit_path);
		//			SafeDelete(pFInf);
		//			return false;
		//		}
		//	}

		//	return true;;
		//}
		//SQR_CATCH(exp)
		//{
		//	exp.AppendMsg("CFileFormatMgr创建异常");
		//	LogExp(exp);
		//	SafeDelete(pFInf);
		//}
		//SQR_TRY_END;
		//return false;

		return true;
	}

	void CEditFormatBase::_Load(OBJ_HANDLE handle, CRefBufFile& File)
	{
		EditStruct edit_struct = _getEditStruct(handle);

		// read binary data
		CFileFormatMgr::_Load(handle, File);
		
		// read xml data
		string			xml_buf;
		size_t			xml_part_size	= File.GetBufSize() - File.Tell();
		const char*		raw_buf			= File.GetBuf() + File.Tell();

		xml_buf.resize(xml_part_size);
		memcpy(&xml_buf[0], raw_buf, xml_part_size);
		stringstream ss(xml_buf);
		ss >> (*edit_struct.edit_info->getXmlDoc());
		TiXmlDocument* xml_doc = edit_struct.edit_info->getXmlDoc();
		
		// read reference info
		TiXmlNode* ref_root_node = xml_doc->FirstChild(CEditInfo::REF_ROOT_NODE);
		if ( ref_root_node != 0)
		{
			TiXmlElement* ref_elem = ref_root_node->FirstChildElement();
			while ( ref_elem != 0 )
			{
				edit_struct.edit_info->m_ref_parent_set.insert(ref_elem->GetText());
				ref_elem = ref_elem->NextSiblingElement();
			}
		}
	}

	void CEditFormatBase::pathFilter(const string& path, PathGroup& path_group)
	{
		const string postfix("_s");

		path_group.live_path = path;
		if ( path_group.live_path.rfind(postfix) == path_group.live_path.length() - postfix.length() )
			path_group.live_path = path_group.live_path.substr(0, path_group.live_path.rfind(postfix));
		/*CPathBrowser::GetResFileName(path_group.dead_path, path_group.live_path);
		
		const string SRC_FOLDER_NAME = CPathBrowser::SRC_FOLDER_NAME;
		const string TGT_FOLDER_NAME = CPathBrowser::TGT_FOLDER_NAME;

		size_t pos = path_group.dead_path.rfind(SRC_FOLDER_NAME);
		size_t len = path_group.dead_path.length() - SRC_FOLDER_NAME.length();
		if ( pos == len || pos == len-1  )
		{
			path_group.dead_path = path_group.dead_path.substr(0, path_group.dead_path.rfind(SRC_FOLDER_NAME));
		}
		else
		{
			path_group.dead_path = path_group.dead_path.substr(0, path_group.dead_path.rfind(TGT_FOLDER_NAME));
		}

		path_group.edit_path = path_group.dead_path + CPathBrowser::SRC_FOLDER_NAME + "/" + path_group.live_path + postfix;
		path_group.game_path = path_group.dead_path + CPathBrowser::TGT_FOLDER_NAME + "/" + path_group.live_path;
		*/
	}

	void CEditFormatBase::createSaveDirectory(string path)
	{
		slash_to_backslash(path);
		path = path.substr(0, path.rfind("\\"));
		MakeDirectory(gbk_to_utf16(path).c_str());
	}

}

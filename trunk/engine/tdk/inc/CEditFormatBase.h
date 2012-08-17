#pragma once
#include "CFileFormatMgr.h"

class TiXmlDocument;

namespace sqr_tools
{
	class CEditInfo;

	class CEditFormatBase : public CFileFormatMgr
	{
		struct PathGroup
		{
			string dead_path;	// 绝对路径
			string live_path;	// 相对路径
			string edit_path;	// 编辑数据存储路径
			string game_path;	// 游戏数据存储路径
		};

		friend class CEditInfo;

	protected:
		struct EditStruct
		{
			CEditInfo*	edit_info;
			void*		obj_ptr;
		};

	public:
		CEditFormatBase();
		virtual ~CEditFormatBase();

		bool				Save(const string& file_name, OBJ_HANDLE handle);
		bool				Load(const string& file_name, OBJ_HANDLE handle);

	private:
		virtual string		_nameFilter(const string& name) { return string(name); }
		virtual void		_registerRefernceInfo(OBJ_HANDLE handle) {}
		virtual bool		_saveGameData(const string& file_name, OBJ_HANDLE handle)	= 0;
		virtual bool		_loadGameData(const string& file_name, OBJ_HANDLE handle)	= 0;
		virtual EditStruct	_getEditStruct(OBJ_HANDLE handle)							= 0;

	// ---- implementation ---------------------------------------------------------------------------

	private:
		void				pathFilter(const string& path, PathGroup& path_group);
		void				createSaveDirectory(string path);
		
		void				_Load(OBJ_HANDLE handle, CRefBufFile& File);
		bool				saveGameData(const string& file_name, CEditInfo* handle);
		bool				saveEditData(const string& file_name, CEditInfo* handle);
		bool				isOpenResbin(const string& real_path, const string& cur_path);
	};		

}

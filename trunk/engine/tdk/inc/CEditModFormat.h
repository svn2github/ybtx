#pragma once
#include "CEditFormatBase.h"

namespace sqr_tools
{
	class CEditModFormat : public CEditFormatBase
	{
	public:
		CEditModFormat();
		~CEditModFormat();

	private:
		string			_nameFilter(const string& name);
		void			_registerRefernceInfo(OBJ_HANDLE handle);
		bool			_saveGameData(const string& file_name, OBJ_HANDLE handle);
		bool			_loadGameData(const string& file_name, OBJ_HANDLE handle);
		
		EditStruct		_getEditStruct(OBJ_HANDLE handle);
	};
}

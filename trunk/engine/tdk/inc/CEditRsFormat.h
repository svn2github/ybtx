#pragma once
#include "CEditFormatBase.h"

namespace sqr_tools
{
	class CEditRsFormat : public CEditFormatBase
	{
	private:
		string		_nameFilter(const string& name);
		void		_registerRefernceInfo(OBJ_HANDLE handle);
		bool		_saveGameData(const string& file_name, OBJ_HANDLE handle);
		bool		_loadGameData(const string& file_name, OBJ_HANDLE handle);
		EditStruct	_getEditStruct(OBJ_HANDLE handle);
	};
}
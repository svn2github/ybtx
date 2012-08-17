#pragma once
#include "CEditFormatBase.h"

namespace sqr_tools
{
	class CEditTexFormat : public CEditFormatBase
	{
	private:
		bool		_saveGameData(const string& file_name, OBJ_HANDLE handle);
		bool		_loadGameData(const string& file_name, OBJ_HANDLE handle);

		EditStruct	_getEditStruct(OBJ_HANDLE handle);
	};
}
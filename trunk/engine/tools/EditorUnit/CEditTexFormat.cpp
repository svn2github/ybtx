#include "stdafx.h"
#include "CEditTexFormat.h"
#include "CEditTexture.h"

namespace sqr_tools
{
	bool CEditTexFormat::_saveGameData( const string& file_name, OBJ_HANDLE handle )
	{
		return true;
	}

	bool CEditTexFormat::_loadGameData( const string& file_name, OBJ_HANDLE handle )
	{
		return true;
	}

	CEditFormatBase::EditStruct CEditTexFormat::_getEditStruct( OBJ_HANDLE handle )
	{
		CEditFormatBase::EditStruct edit_struct;
		edit_struct.edit_info	= (CEditInfo*)(CEditTexture*)handle;
		edit_struct.obj_ptr		= handle;
		return edit_struct;
	}
}

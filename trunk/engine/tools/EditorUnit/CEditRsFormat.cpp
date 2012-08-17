#include "stdafx.h"
#include "CEditRsFormat.h"
#include "CEditRenderStyle.h"

namespace sqr_tools
{
	std::string CEditRsFormat::_nameFilter( const string& name )
	{
		if ( name.rfind(".marl") == string::npos )
			return string(name + ".marl");
		else
			return name;
	}

	void CEditRsFormat::_registerRefernceInfo( OBJ_HANDLE handle )
	{

	}

	bool CEditRsFormat::_saveGameData( const string& file_name, OBJ_HANDLE handle )
	{
		CRenderStyle* render_style = (CRenderStyle*)(CEditRenderStyle*)handle;

		return true;
	}

	bool CEditRsFormat::_loadGameData( const string& file_name, OBJ_HANDLE handle )
	{
		CRenderStyle* render_style = (CRenderStyle*)(CEditRenderStyle*)handle;

		return true;
	}

	CEditFormatBase::EditStruct CEditRsFormat::_getEditStruct( OBJ_HANDLE handle )
	{
		CEditFormatBase::EditStruct edit_struct;
		edit_struct.edit_info = (CEditInfo*)(CEditRenderStyle*)handle;
		edit_struct.obj_ptr	  = handle;

		return edit_struct;
	}
}
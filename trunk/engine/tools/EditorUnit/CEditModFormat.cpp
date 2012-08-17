#include "stdafx.h"
#include "CEditModFormat.h"
#include "CPieceGroup.h"
#include "CEditPieceGroup.h"
#include "CPieceGroup.h"
#include "CEditTexture.h"
#include "CEditModCoder.h"
#include "CEditRenderStyle.h"

namespace sqr_tools
{
	CEditModFormat::CEditModFormat()
	{
		//RegisterCoder(new CEditModCoder());
	}

	CEditModFormat::~CEditModFormat()
	{
	}

	string CEditModFormat::_nameFilter(const string& name)
	{
		if ( name.rfind(".mod") == string::npos )
			return string(name + ".mod");
		else
			return string(name);
	}

	void CEditModFormat::_registerRefernceInfo(OBJ_HANDLE handle)
	{
		CEditPieceGroup* model_data = (CEditPieceGroup*)(CEditInfo*)handle;

		for ( size_t pc_index = 0; pc_index<model_data->GetPieceClassNum(); ++pc_index )
		{
			CPieceClass* piece_class = model_data->GetPieceClass(pc_index);

			CPieceClass::iterator piece_iter = piece_class->begin();
			for ( ; piece_iter != piece_class->end(); ++piece_iter )
			{
				CPiece* piece = (*piece_iter);
				for ( size_t rs_index = 0; rs_index < piece->GetRSNum(); ++rs_index )
				{
					CEditPieceRenderStyle* prs = (CEditPieceRenderStyle*)piece->GetRS(rs_index);
					for ( int tex_index = 0; tex_index < prs->GetTextureNum(); ++tex_index )
					{
						CEditTexture*		tex = (CEditTexture*)prs->GetTexture(tex_index);
						CEditRenderStyle*	ers	= prs->GetEditRenderStyle();
						model_data->RefersTo(tex);
						model_data->RefersTo(ers);
					}
				}
			}
		}
	}

	bool CEditModFormat::_saveGameData( const string& file_name, OBJ_HANDLE handle )
	{
		CPieceGroup* model_data = (CPieceGroup*)(CEditPieceGroup*)handle;
		
		return true;
	}

	bool CEditModFormat::_loadGameData( const string& file_name, OBJ_HANDLE handle )
	{
		CPieceGroup* model_data = (CPieceGroup*)(CEditPieceGroup*)handle;

		return true;
	}

	CEditModFormat::EditStruct CEditModFormat::_getEditStruct( OBJ_HANDLE handle )
	{
		EditStruct edit_struct;
		edit_struct.edit_info	= (CEditInfo*)(CEditPieceGroup*)handle;
		edit_struct.obj_ptr		= handle;	

		return edit_struct;
	}
}

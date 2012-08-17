#pragma once
#include "CRenderDialog.h"

namespace sqr
{
	class CTObject;

	class CMapObjBaseDialog : public CRenderDialog
	{
	protected:
		string    m_strGuiAucName;
		CTObject *m_pEditorObject;

	public:
		CMapObjBaseDialog();
		CMapObjBaseDialog(const string& aucname, CTObject *pObject);
		~CMapObjBaseDialog();

		virtual void CreateObjRenderDialog(CTObject *pObject);
		virtual void OnLButtonDown( uint32 nFlags, int32 x, int32 y ) {;}

		void	SetDialogWndImageParam(const string& wndName, const string& texname, const uint32 uColor);
		void	SetDialogWndText(const string& wndName, const string& text);
	};
}
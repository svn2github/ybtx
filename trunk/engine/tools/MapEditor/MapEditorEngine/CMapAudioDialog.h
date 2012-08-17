#pragma once
#include "CMapObjBaseDialog.h"

namespace sqr
{
	class CTObject;

	class CMapAudioDialog  : public CMapObjBaseDialog
	{
	public:
		CMapAudioDialog();
		CMapAudioDialog(CTObject *pObject);
		~CMapAudioDialog();

		virtual void OnLButtonDown( uint32 nFlags, int32 x, int32 y );
	};
}
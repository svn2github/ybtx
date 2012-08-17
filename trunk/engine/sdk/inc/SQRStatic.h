
#pragma once
#include "SQRControl.h"

namespace sqr
{
	class GUI_API SQRStatic : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRStatic )
	public:
		SQRStatic(void);
		virtual ~SQRStatic(void);

		virtual void DrawText( const wstring& text, uint32 textColor, uint32 textBcColor );
	};

}
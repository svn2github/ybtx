#pragma once
#include "SQRButton.h"

namespace sqr
{
	class GUI_API SQRIcon : public SQRButton
	{
		DYNAMIC_DECLARE_WND( SQRIcon )

	public:
		SQRIcon(void);
		virtual ~SQRIcon(void);

	protected:
		virtual void	OnLButtonDblClk( uint32 nFlags, int32 x, int32 y );
	};

}
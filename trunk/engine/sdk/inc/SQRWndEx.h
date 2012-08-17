
#pragma once
#include "SQRControl.h"

namespace sqr
{
	class SQRWndEx : public SQRControl
	{
	public:
		SQRWndEx();
		~SQRWndEx();

	protected:
		//×ó¼üµã»÷ÏûÏ¢
		virtual void	OnLButtonDown( unsigned int nFlags, int x, int y );
	};
}
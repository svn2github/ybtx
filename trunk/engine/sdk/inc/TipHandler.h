#pragma once
#include "SQRRichToolTip.h"
#include "CDynamicObject.h"
#include "CallBackX.h"
#include "SQRGUIMallocObject.h"

namespace sqr
{
	class SQRRichToolTip;
	class TipHandler
		: public virtual CDynamicObject
	{
	public:
		TipHandler();
		~TipHandler();
		virtual void OnToolTipShow( SQRRichToolTip* tip ) { CALL_CLASS_CALLBACK_1(tip) }

		virtual void ActiveRichToolTip( SQRRichToolTip* tip );
		virtual void DelRichToolTip( SQRRichToolTip* tip );
	private:
		UISet<SQRRichToolTip*> m_RichToolTipSet;
	};
}
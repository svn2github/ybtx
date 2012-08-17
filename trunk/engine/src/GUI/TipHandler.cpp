#include "stdafx.h"
#include "TipHandler.h"
#include "TSqrAllocator.inl"

TipHandler::TipHandler()
{
	m_RichToolTipSet.clear();
}

TipHandler::~TipHandler()
{
	UISet<SQRRichToolTip*>::iterator itr = m_RichToolTipSet.begin();
	UISet<SQRRichToolTip*>::iterator itrend = m_RichToolTipSet.end();
	while (itr != itrend)
	{
		if (*itr)
			(*itr)->ClearTipHandler();
		++itr;
	}
	m_RichToolTipSet.clear();
}

void TipHandler::ActiveRichToolTip( SQRRichToolTip* tip )
{
	if ( tip )
		m_RichToolTipSet.insert(tip);
}

void TipHandler::DelRichToolTip( SQRRichToolTip* tip )
{
	UISet<SQRRichToolTip*>::iterator itr = m_RichToolTipSet.find(tip);
	if ( itr != m_RichToolTipSet.end() )
		m_RichToolTipSet.erase(itr);
}
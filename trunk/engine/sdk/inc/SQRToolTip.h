#pragma once
#include "SQRRichText.h"

namespace sqr
{
	class SQRToolTip 
		: public SQRRichString
	{
	public:
		SQRToolTip();
		~SQRToolTip();

		WND_IMAGE_LIST	m_Image;
		uint32			m_uStyle;
		UIWString		m_szDescW;
		UIString		m_szDesc;
		CFPos			m_DrawPos;
		bool			m_bTexInit;

		virtual void	SetToolTipDesc( const char* szDesc );
		virtual bool	AddChildToolTip(void);
		virtual	void	ClearChildToolTip(void);
		virtual SQRToolTip* GetChildToolTip(void);
		virtual void	SetToolTipStyle(uint32 uStyle);	

		static void		DrawToolTip(SQRWnd* pWnd);
		static void		PrepareToolTip(SQRWnd* pWnd);

	protected:
		SQRToolTip*		m_pChildToolTip;
		
	};
}
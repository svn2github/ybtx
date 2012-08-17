#pragma once
#include "SQRGUIEffectBase.h"
#include "SQRWnd.h"

// ½¥Òþ½¥ÏÔÐ§¹û
namespace sqr
{
	class SQRGUITransparentEffect : public SQRGUIEffectBase
	{
	public:
		SQRGUITransparentEffect(SQRWnd* wnd);
		~SQRGUITransparentEffect();

		virtual void		ExecuteEffect();
		virtual EGUIEffect	GetEffectType();
		void				SetTransparentObj(int32 runtime, bool bvalue);
		void				EndTransparent( bool bvalue );
		virtual void		EndEffect();
	private:
		void				_EndTransparent( bool bvalue);
		void				_SetTransparent( float fTrans );
	private:
		SQRWnd*			m_wnd;

		DWORD			m_systime;
		INT32			m_runtime;

		FLOAT			m_TransparentValue;
		FLOAT			m_OldTransparentValue;
		bool			m_bValue;
	};
}
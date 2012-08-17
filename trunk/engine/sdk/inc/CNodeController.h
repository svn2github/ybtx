#pragma once
#include "CGraphicMallocObject.h"
namespace sqr
{
	enum CtrlState
	{
		CS_BEGIN,
		CS_STOP,
		CS_CTRL,
		CS_SPECIAL,
		CS_END,
	};
	class CCtrlNode;
	
	class CNodeController : public CGraphicMallocObject
	{
	protected:
		static	const uint32 INVALID_INDEX;
	public:
		CNodeController();
		virtual ~CNodeController();

		void		Bind(CCtrlNode* pNode);
		void		UnBind(void);

		void		BeginCtrl(uint32 uCurTime);
		void		EndCtrl(void);

		CtrlState	DoCtrl( uint32 uCurTime );
	protected:
		bool		m_isInCtrl;
		uint32		m_uStartTime;
		uint32		m_uCurTime;
		CCtrlNode*	m_pCtrlNode;

	protected:
		virtual bool _IsValid( CCtrlNode* pNode ) { return true; };
		virtual void _InitBind(void)	{}
		virtual CtrlState _DoCtrl( uint32 uCurTime ) = 0;
	};
}
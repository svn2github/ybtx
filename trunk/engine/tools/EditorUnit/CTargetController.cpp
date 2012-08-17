#include "stdafx.h"
#include "CCtrlNode.h"
#include "BaseHelper.h"
#include "CTargetController.h"
#include "CVector3.h"
#include "CMath.h"

namespace sqr_tools
{
	class CTargetControllerImp
	{
	public:
		CTargetControllerImp()
		{
		}
		~CTargetControllerImp()
		{
		}

		CVector3f	target;
	};

	CTargetController::CTargetController()
		: m_imp(new CTargetControllerImp())
	{
	}

	CTargetController::~CTargetController()
	{
		SafeDelete(m_imp);
	}

	void CTargetController::SetTargetPos( const CVector3f& target )
	{
		m_imp->target = target;
		setNodeDir();
	}

	const CVector3f& CTargetController::GetTargetPos() const
	{
		return m_imp->target;
	}

	sqr::CtrlState CTargetController::_DoCtrl( uint32 uCurTime )
	{
		return CS_CTRL;
	}

	void CTargetController::RatateByTgtAndAxis(const CVector3f& target, const CVector3f& axis, float angle)
	{
		if ( !m_pCtrlNode )
			return;

		m_pCtrlNode->centerRotate(target, axis, angle);
	}

	void CTargetController::_InitBind( void )
	{
	}

	void CTargetController::setNodeDir()
	{
		if ( !m_pCtrlNode || m_pCtrlNode->GetPosition() == m_imp->target )
			return;
		
		CVector3f dir = m_imp->target - m_pCtrlNode->GetPosition();
		dir.Normalize();
		m_pCtrlNode->setDIRECTION(dir);
	}
}

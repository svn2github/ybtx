#include "CRecordCamera.h"

namespace sqr
{
	inline void CRecordCamera::SetType( EType eType )
	{
		type= eType;
	}

	inline void CRecordCamera::MoveLeft()
	{
		CVector3f vecDir = -((CCamera*)m_pCtrlNode)->getRIGHT();
		Move(vecDir);
	}

	inline void CRecordCamera::MoveRight()
	{
		CVector3f vecDir = ((CCamera*)m_pCtrlNode)->getRIGHT();
		Move(vecDir);
	}

	inline void CRecordCamera::MoveFront()
	{
		CVector3f vecDir = ((CCamera*)m_pCtrlNode)->getDIRECTION();
		Move(vecDir);
	}

	inline void CRecordCamera::MoveBack()
	{
		CVector3f vecDir = -((CCamera*)m_pCtrlNode)->getDIRECTION();
		Move(vecDir);
	}

	inline CRecordCameraNode* CRecordCamera::GetEventPorcess() const
	{
		return EventProc;
	}
}
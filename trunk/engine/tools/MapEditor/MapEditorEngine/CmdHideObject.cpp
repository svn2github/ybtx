#include "stdafx.h"
#include "CmdHideObject.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"
#include "OperationLog.h"

namespace sqr
{
	void CCmdHideObject::_Execute()
	{
		

		m_diff.pObject->GetModelGroup()->SetHide(m_diff.bHide);
		m_diff.bHide = !m_diff.bHide;

		COpearationLog::GetInst()->WriteOperationLog("DO Hide Object");
	}

	void CCmdHideObject::_UnExecute()
	{
		// back to stationary state
		

		m_diff.pObject->GetModelGroup()->SetHide(m_diff.bHide);
		m_diff.bHide = !m_diff.bHide;

		COpearationLog::GetInst()->WriteOperationLog("UNDO Hide Object");
	}

	void CCmdHideObject::SetObject(CTObject * p)
	{
		m_diff.pObject = p;
	}

	void CCmdHideObject::SetBHideModel(bool bhide)
	{
		m_diff.bHide = bhide;
	}
}
#include "stdafx.h"
#include "CmdLiftMoveObject.h"
#include "CMapEditObject.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"

namespace sqr
{
	void CCmdLiftMoveObject::_Execute()
	{
	}

	void CCmdLiftMoveObject::_UnExecute()
	{
		CMapEditObject * pModel = NULL;
		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		if ( pModel == NULL )
			return;

		pModel->SetShiftXEx(m_diff.fShiftXEx);
		pModel->SetShiftZEx(m_diff.fShiftZEx);
		pModel->SetCenterGridIndex(m_diff.nCenterGridIndex);
	}
}
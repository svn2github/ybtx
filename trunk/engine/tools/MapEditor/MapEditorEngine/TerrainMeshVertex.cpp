#include "StdAfx.h"
#include "TerrainMeshVertex.h"
#include "WireBox.h"
#include "ExsertStructDB.h"

namespace sqr
{
	SVertex::SVertex(void)
	{
		pWireBoxLogicHeight = NULL;
		bHasCalculate		= false;
		vNormal				= CVector3f(0.0, 1.0f, 0.0f);
		fLogicHeight		= 0.0f;
	}

	SVertex::~SVertex(void)
	{
		if( pWireBoxLogicHeight )
			pWireBoxLogicHeight = NULL;
	}

	CWireBox * SVertex::GetWireBoxLogicHeight()
	{
		if ( !pWireBoxLogicHeight )
		{
			pWireBoxLogicHeight = CSceneManager::GetInst()->AddNodeWireBox(0.0f, GRID_SPACE/4,GRID_SPACE/4, 0xffff0000, false);
		}

		return pWireBoxLogicHeight;
	}
}
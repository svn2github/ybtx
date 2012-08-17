#include "StdAfx.h"
#include "CreateSceneObjectMgr.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "ToolSetModel.h"
#include "CMapEffect.h"

CreateSceneObjectMgr::CreateSceneObjectMgr(void)
{
}

CreateSceneObjectMgr::~CreateSceneObjectMgr(void)
{
}

bool sqr::CreateSceneObjectMgr::Rebirth( MoldeSaveProperty* pModelSavePro, const string& strItemName, const string strResSetName, const string& strSkeName, const string& strEffectName, int nAffectRatio, const string strCurName, bool bCameraOriented, bool bEffectBase, bool bAudio, bool bLogin )
{
	sqr::ECenter_Type eCenterType = sqr::ECenter_Type::ECT_CENTER;
	CVector3f vGroupOffset;
	vector<string> veCMapEditModelFileNames;
	veCMapEditModelFileNames.push_back(strResSetName);

	CToolSetModel::GetInst()->CreateActiveObject(false, pModelSavePro->bVisibleByConfigure, pModelSavePro->bShadow, pModelSavePro->bTranslucent, strItemName, veCMapEditModelFileNames, false, eCenterType, 0.0f, 0.0f, 0.0f, pModelSavePro->vUserOffsetEx.y, pModelSavePro->vUserOffsetEx.x, pModelSavePro->vUserOffsetEx.z, 
		pModelSavePro->vRotate.y, 0, pModelSavePro->vRotate.z, 0, pModelSavePro->vRotate.x, 0, pModelSavePro->vScale.x, pModelSavePro->vScale.y, pModelSavePro->vScale.z, vGroupOffset, bCameraOriented, bEffectBase, bAudio, bLogin);

	CEditModelGroup *pModelGroup = NULL;
	CTObject *pTObject = NULL;
	pTObject = CToolSetModel::GetInst()->GetLastActiveObject();
	if ( pTObject != NULL )
	{
		pModelGroup = pTObject->GetModelGroup();
		if( pModelGroup == NULL )
			return false;

		if ( !strSkeName.empty() && !bEffectBase )
			pModelGroup->SetAnimationByAgpFileName(strSkeName);

		if( !pModelSavePro->bPlayAnimate && !strSkeName.empty() )
			pModelGroup->StopModelAnimation();
	}
	else
		return false;

	if( !strEffectName.empty() )
	{
		CMapEffect::GetInst()->ToolSetModel_LinkEffect(strEffectName,CMapEffect::GetInst()->GetLocalFXGroup(strEffectName), NULL);
	}

	if( !CToolSetModel::GetInst()->Rebirth(pModelSavePro, nAffectRatio, strCurName) )
		return false;

	return true;
}
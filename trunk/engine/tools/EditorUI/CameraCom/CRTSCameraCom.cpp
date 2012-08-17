#include "stdafx.h"
#include "CRTSCameraCom.h"
#include "CameraCtrl/CRTSCameraCtrl.h"

CRTSCameraCom::CRTSCameraCom()
{

}

CRTSCameraCom::~CRTSCameraCom()
{

}

char*	CRTSCameraCom::GetImpTypeName(void)
{
	return GET_IMP_NAME(CRTSCameraCtrl);
}


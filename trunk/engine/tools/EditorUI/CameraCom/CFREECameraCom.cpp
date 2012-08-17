#include "stdafx.h"
#include "CFREECameraCom.h"
#include "CameraCtrl/CFREECameraCtrl.h"

CFREECameraCom::CFREECameraCom(void)
{
}

CFREECameraCom::~CFREECameraCom(void)
{
}

char* CFREECameraCom::GetImpTypeName(void)
{
	return GET_IMP_NAME(CFREECameraCtrl);
}

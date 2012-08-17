#include "stdafx.h"
#include "CFPSCameraCom.h"
#include "CameraCtrl/CRTSCameraCtrl.h"

CFPSCameraCom::CFPSCameraCom()
{

}

CFPSCameraCom::~CFPSCameraCom()
{

}

char*	CFPSCameraCom::GetImpTypeName(void)
{
	return GET_IMP_NAME(CFPSCameraCtrl);
}

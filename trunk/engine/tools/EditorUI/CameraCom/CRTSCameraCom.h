#pragma once
#include "CCameraBaseCom.h"

namespace sqr_tools
{
	public ref class CRTSCameraCom : public CCameraBaseCom
	{
	public:
		CRTSCameraCom();
		~CRTSCameraCom();
	public:  virtual char*	GetImpTypeName(void)	override;
	};
}

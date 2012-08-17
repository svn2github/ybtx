#pragma once
#include "CCameraBaseCom.h"

namespace sqr_tools
{
	public ref class CFPSCameraCom : public CCameraBaseCom
	{
	public:
		CFPSCameraCom();
		~CFPSCameraCom();
	public:  virtual char*	GetImpTypeName(void)	override;
	};
}

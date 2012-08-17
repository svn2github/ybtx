#pragma once
#include "CCameraBaseCom.h"

namespace sqr_tools
{
	public ref class CFREECameraCom : public CCameraBaseCom
	{
	public:
		CFREECameraCom(void);
		~CFREECameraCom(void);
	public:  virtual char*	GetImpTypeName(void)	override;
	};

//!namespace
}
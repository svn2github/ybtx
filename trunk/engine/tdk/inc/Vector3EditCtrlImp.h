#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CVecCmd;
	class Vector3EditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(Vector3EditCtrlImp);
		friend class CVecCmd;
	public:
		Vector3EditCtrlImp(void* param);
		~Vector3EditCtrlImp();

	public:
		virtual void	SetImpXValue(float x);
		virtual void	SetImpYValue(float y);
		virtual void	SetImpZValue(float z);

		virtual float	GetImpXValue();
		virtual float	GetImpYValue();
		virtual float	GetImpZValue();
	private:
		virtual void	_SetImpXValue(float x);
		virtual void	_SetImpYValue(float y);
		virtual void	_SetImpZValue(float z);
		float		m_fz;
	};
}
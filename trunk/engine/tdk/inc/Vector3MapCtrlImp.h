#pragma once
#include "CCtrlPropBase.h"
#include "Vector3EditCtrlImp.h" 

namespace sqr_tools
{
	class CVecMapInsCmd;
	class CVecMapDelCmd;
	class Vector3MapCtrlImp : public Vector3EditCtrlImp
	{
		REG_UI_IMP(Vector3MapCtrlImp);
		friend class CVecMapInsCmd;
		friend class CVecMapDelCmd;
	public:
		Vector3MapCtrlImp(void* param);
		~Vector3MapCtrlImp();

		virtual int				GetVector3MapCount();
		virtual float			GetVector3MapPer(int n);
		virtual float			GetVector3MapXVal(float idx);
		virtual float			GetVector3MapYVal(float idx);
		virtual float			GetVector3MapZVal(float idx);

		virtual void			SetCurPercent(float per);
		virtual float			GetCurPercent();
		virtual CVector3f*		GetVector3Node(float idx);

		virtual void			SetCurVecToPer(float per);
		virtual void			DelCurPerNode();
		virtual void			InsertNewNode(float idx);


		virtual float			GetImpXValue();
		virtual float			GetImpYValue();
		virtual float			GetImpZValue();
	
	private:
		virtual void	_SetImpXValue(float x);
		virtual void	_SetImpYValue(float y);
		virtual void	_SetImpZValue(float z);

		float		m_fCurPercent;
	};
}
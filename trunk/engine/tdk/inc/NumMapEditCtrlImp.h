#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CNumSetCmd;
	class CNumDelCmd;
	class CNumSetMaxCmd;
	class CNumSetMinCmd;
	class CCombinCmd;
	class NumMapEditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(NumMapEditCtrlImp);

		friend class CNumSetCmd;
		friend class CNumDelCmd;
		friend class CNumSetMaxCmd;
		friend class CNumSetMinCmd;
	public:
		NumMapEditCtrlImp(void* param);
		~NumMapEditCtrlImp();

		virtual void			SetEditCtrlData(void* ctrldata);
		virtual int				GetNumMapCount();
		virtual float			GetNumMapIdx(int i);
		virtual float			GetNumMapVal(int i);
		virtual float			GetNumMapValPercent(int i);
		virtual float			GetCurPercent();
		virtual float			GetZeroPercent();

		virtual void			DelCurPerNode();
		virtual void			SetNumMapVal(float idx, float per);
		virtual void			SetCurPercent(float per);
		virtual void			SetCurNodeVal(float per);

		virtual float			GetMaxVal(void);
		virtual float			GetMinVal(void);

		virtual void			SetMaxVal(float max);
		virtual void			SetMinVal(float min);
	private:
		virtual void			_UpdateMaxMin();
		virtual void			_ResetMapData(CCombinCmd* pCmd,float min,float max);
	public:
		float		m_fCurPercent;
		float		m_fMaxVal;
		float		m_fMinVal;
	};
}
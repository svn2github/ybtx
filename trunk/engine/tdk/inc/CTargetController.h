#pragma once
#include "CNodeController.h"

namespace sqr
{
	template<class T>
	struct TVector3;
	typedef TVector3<float> CVector3f;
}

namespace sqr_tools
{
	class CTargetControllerImp;

	class CTargetController : public CNodeController
	{
	public:
		CTargetController();
		~CTargetController();

		void				SetTargetPos(const CVector3f& target);
		const CVector3f&	GetTargetPos() const;
	
		void				RatateByTgtAndAxis(const CVector3f& target, const CVector3f& axis, float angle);

	protected:
		CtrlState			_DoCtrl(uint32 uCurTime);
		virtual void		_InitBind(void);

	private:
		CTargetController(const CTargetController&);
		CTargetController& operator=(const CTargetController&);

		void				setNodeDir();

	private:
		CTargetControllerImp*	m_imp;
	};
}
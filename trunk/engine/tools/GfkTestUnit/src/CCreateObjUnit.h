#pragma once
#include "CBaseUnit.h"
#include "CEditModel.h"
namespace sqr_tools
{
	class CCreateObjUnit : public CBaseUnit
	{
		DEF_TEST_UNIT(CCreateObjUnit);
	public:
		CCreateObjUnit(CEditContext* pContext);
		~CCreateObjUnit();
		void		_OnTick(void);
	
		void		_InitUnit();
		void		_UnInitUnit();

		void		ReSizeVector(void);
	protected:
		uint32		m_uCount;
		uint32		m_uNumSp;
		uint32		m_uTime;
		bool		m_bCamera;
		float		m_fCenterX;
		float		m_fCenterY;
		float		m_fRange;

		CEditModel*	_CreateNewModel(void);

	protected:
		uint32		m_BackIdx;
	};
}

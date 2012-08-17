#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	class CProgressCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CProgressCtrlImp);
	public:
		CProgressCtrlImp(void* param);
		~CProgressCtrlImp(void);

	private:
		int m_nStep;
		int m_nValue;
		int m_nMax;
		int m_nMin;

	public:

		virtual void SetProgressStep(const int nStep);
		virtual int  GetProgressStep() const;

		virtual void SetProgressValue(const int nValue);
		virtual int  GetProgressValue() const;

		virtual void SetProgressMaxValue(const int nMax);
		virtual int  GetProgressMaxValue() const;

		virtual void SetProgressMinValue(const int nMin);
		virtual int  GetProgressMinValue() const;

		virtual void Update();

	};

}

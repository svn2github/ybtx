#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	class CPortraitView;
	class CPortraitViewCtrlImp : public CCtrlBase
	{
		REG_UI_IMP(CPortraitViewCtrlImp);
	public:
		CPortraitViewCtrlImp(void* param);
		~CPortraitViewCtrlImp(void);

		void SetPhotoFrameTexPath(const std::string& path);
		void GetPhotoFrameTexPath(std::string& path);

		void SetIsShowPhotoFrame(const bool b);
		bool GetIsShowPhotoFrame() const;

		void SetIsShowPortraitView(const bool b);
		bool GetIsShowPortraitView() const;

		void SetPortraitView(CPortraitView* pPortraitView);
	private:
		CPortraitView*	m_pPortraitView;
	};
}
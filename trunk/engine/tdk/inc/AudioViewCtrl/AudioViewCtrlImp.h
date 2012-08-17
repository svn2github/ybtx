#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	class CCueListCtrlImp;
	class CAmbientAudioCtrlImp;

	class CAudioViewCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CAudioViewCtrlImp);

		CCueListCtrlImp*		m_pCueListCtrlImp;
		CAmbientAudioCtrlImp*	m_pAmbientAudioCtrlImp;

	public:
		CAudioViewCtrlImp(void* param);
		~CAudioViewCtrlImp(void);

	public:

 		virtual void Update();
 		virtual bool _ProcMsg(const CMsg& msg );
		virtual void SetContext(CEditContext* pContext);

		virtual void SetCueListCtrlImp(CCueListCtrlImp* pImp);
		virtual void SetAmbientAudioCtrlImp(CAmbientAudioCtrlImp* pImp);

		virtual void PlayCurrentCue();
		virtual void StopCurrentCue();
		virtual void CreateAmbientAudio();
	};

}

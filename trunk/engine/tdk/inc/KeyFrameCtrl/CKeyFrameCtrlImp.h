#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr_tools
{
	class CKeyFrameCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CKeyFrameCtrlImp);
	public:
		CKeyFrameCtrlImp(void* param);
		~CKeyFrameCtrlImp(void);

	public:
		virtual void ActiveNode( CRenderNode* pNode );
		virtual void   SetKeyFrame(const string& key);
		virtual string GetKeyFrame();
		virtual std::string SaveKeyframeInfo(void);
		virtual void Update();

	private:
		string m_strKeyFrame;
	};

}

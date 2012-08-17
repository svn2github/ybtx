#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr
{
	class IDNAME;
}

namespace sqr_tools
{
	class CEditPieceGroup;
	class CEditModel;
	class CEditSkeletalFrame;
	class CEditAnimationGroup;
	class CEditAnimate;

	class CAnimationGroupEditImp : public CCtrlBaseObserver
	{
		REG_UI_IMP(CAnimationGroupEditImp);
	public:
		CAnimationGroupEditImp(void* param);
		~CAnimationGroupEditImp(void);

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual std::string AddAgp(std::string str);
		virtual void ClearAgp(void);
		virtual std::string SaveAgp(std::string str);
		virtual void ChangeAnimation(std::string str);
		virtual int  GetCount(void);
		virtual std::string  CalName(sqr::IDNAME name,float fSpeed);
		virtual std::string  GetAnimNameByIndex(int index);
		virtual void ChangeCurSelSkeBoneName(std::string str);
		virtual bool IsNodesChanged(void);
		virtual void UpdateNodes(void);
		virtual string GetLastAGPFile(void);
		virtual void SaveNewAGPFile(std::string name);

	private:
		CEditModel*		m_pModel;			// 被编辑的模型
		std::vector<std::string> m_vtAnimName;
		CEditSkeletalFrame*		m_pSkeletalFrame;
		CEditAnimationGroup*	m_pAnimateGroup;
		CEditAnimate* m_CurAni;
		std::string m_strCurAgpName;
	};
}
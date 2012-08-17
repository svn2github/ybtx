#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr
{
	class CSkeletalFrame;
}

namespace sqr_tools
{
	class CEditModel;
	class CEditPiece;
	class CEditPieceGroup;
	class CSkeletonDelegate;
	class CSkeletonEditImp : public CCtrlBaseObserver
	{
		REG_UI_IMP(CPieceUnitEditImp);
	public:
		CSkeletonEditImp(void* param);
		~CSkeletonEditImp(void);

		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual void SetModel(CEditModel* model);
		virtual std::string  GetCurSkeName(void);
		virtual void SetCurSelSkeBoneName(std::string str);
		virtual std::string GetCurSelSkeBoneName(void);
		virtual void SetShowSkeleton(bool value);
		virtual void SetSkeBoneSize(int delta);
		virtual void ChangeCurSelSkeBoneName(std::string str);
		virtual std::string  SaveSke(void);
		virtual void PickBone(int mouseX, int mouseY);
		virtual void Render(void);

	private:
		CSkeletalFrame* m_pSkeletalFrame;
		CEditModel* m_pModel;
		bool m_bShowSkeleton;
		std::string m_strCurSkeName;
		std::string m_strCurSelSkeBoneName;
		CSkeletonDelegate* m_pDelegate;
	};
}
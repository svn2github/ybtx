#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr_tools
{
	class CEditModel;
	class CEditPiece;
	class CEditPieceGroup;
	class CEditMeshWriter;
	class CBoundingBoxDelegate;
	class CBoundingBoxEditImp : public CCtrlBaseObserver
	{
		REG_UI_IMP(CPieceUnitEditImp);
	public:
		CBoundingBoxEditImp(void* param);
		~CBoundingBoxEditImp(void);

		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual void SetModel(CEditModel* model);
		virtual void SetShowBigBox(bool value);
		virtual void SetShowSmallBox(bool value);
		virtual void SetPieceDirty(bool dirty);
		virtual void RefreshMesh(CEditMeshWriter* pMesh, float xDelta, float yDelta, float zDelta, float xSize, float ySize, float zSize);
		virtual void GetPieceBoxInfo( float& xDelta, float& yDelta, float& zDelta, float& xSize, float& ySize, float& zSize);
		virtual void SetPieceBoxInfo( float xDelta, float yDelta, float zDelta, float xSize, float ySize, float zSize);
		virtual void Render(void);

	private:
		CEditPiece* m_pEditPiece;
		CEditModel* m_pModel;
		bool m_bShowBigBox;
		bool m_bShowSmallBox;
		CBoundingBoxDelegate* m_pDelegate;
	};
}
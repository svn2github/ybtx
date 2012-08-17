#pragma once
#include "CCtrlBaseObserver.h"

namespace sqr_tools
{
	class CEditPieceGroup;
	class CEditModel;

	typedef list<CEditPieceGroup*>	PieceGroupList;
	typedef std::vector<std::string> STRINGVECTOR;
	typedef std::map<std::string, STRINGVECTOR> NODESMAP;

	class CPieceGroupEditImp : public CCtrlBase
	{
		REG_UI_IMP(CPieceGroupEditImp);
	public:
		CPieceGroupEditImp(void* param);
		~CPieceGroupEditImp(void);

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void SetModel( CEditModel* pModel );
		virtual void AddPieceGroup(std::string name, STRINGVECTOR& pieceGroupNames);
		virtual void SaveMesh(std::string name);
		virtual void DeleteAllGroup(void);
		virtual CEditPieceGroup* GetSelPieceGroup(std::string name);
		virtual void DeletePieces();
		virtual void SelectChanged(std::string name);
		virtual string GetLastModelFile(void);
		virtual void SaveNewModelFile(std::string name);

	private:
		CEditModel*		m_pModel;			// 被编辑的模型
		STRINGVECTOR m_vtModelList;
		std::map<std::string, STRINGVECTOR> m_mpNodes;
		std::vector<std::string> m_vtPieceNames;
		PieceGroupList		m_PieceGroupList;
		CEditPieceGroup*	m_pLastEditPieceGroup;
	};
}
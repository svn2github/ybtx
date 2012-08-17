#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr_tools
{
	class CEditModel;
	class CEditPiece;
	class CEditPieceGroup;

	class CPieceUnitEditImp : public CCtrlBaseObserver
	{
		REG_UI_IMP(CPieceUnitEditImp);
	public:
		CPieceUnitEditImp(void* param);
		~CPieceUnitEditImp(void);

	public:
		struct CPieceUnitInfo
		{
			typedef  std::map<std::string, std::vector<std::string>> RSMAP;
			typedef  std::vector<std::string> STRVECTOR;
			CPieceUnitInfo()
			{
				RS_Name = "PieceRS";
				Ani_Name = "VertexAni";
			}

			~CPieceUnitInfo(){}

			std::string name;
			std::string RS_Name;
			std::string Ani_Name;
			RSMAP RS_Names;
			std::vector<std::string> Ani_Names;
		};

		typedef std::vector<CPieceUnitInfo> PieceUnitInfoVector;

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual bool IsPieceGroupChanged(void);
		virtual void UpdateTreeView();
		virtual void SetModel( CEditModel* pModel );
		virtual void AddPieceUnit(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece);
		virtual int	AddPiece(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece );
		virtual int	AddPieceRS(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece );
		virtual int	AddPieceVerAni(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece );
		virtual void DeletePieces();
		virtual void DeletePiece(std::string PieceName);
		virtual void CreateRS(std::string pieceName, std::string& rsName);
		virtual void DeleteRS(std::string pieceName, int rsIndex);
		virtual void SelectPiece( std::string pieceName );
		virtual void SetRSIndexName( std::string pieceName, int rsIndex, std::string name );
		virtual PieceUnitInfoVector& GetPieceUnitInfo(void);
		virtual void SelectChanged(std::string name);

	private:
		CEditModel*		m_pModel;			// 被编辑的模型
		PieceUnitInfoVector m_vtPieceUnitInfo;
	};
}
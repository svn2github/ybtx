#include "stdafx.h"
#include "PieceUnitEditCtrl\CPieceUnitEditImp.h"
#include "BaseHelper.h"
#include "CEditModel.h"
#include "CEditRenderStyle.h"
#include "CEditPieceGroup.h"
#include "CRenderDebugFun.h"
#include "CEditContext.h"
#include "PathManager/CPathManager.h"
#include "ContextMsg.h"

DEF_UI_IMP(CPieceUnitEditImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

sqr::CBaseModel::RenderPieceMap  m_LastPieceMap;

CPieceUnitEditImp::CPieceUnitEditImp(void* param)
: m_pModel(NULL)
{

}

CPieceUnitEditImp::~CPieceUnitEditImp(void)
{

}

void sqr_tools::CPieceUnitEditImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);
}

void sqr_tools::CPieceUnitEditImp::ActiveNode( CRenderNode* pNode )
{
	UpdateTreeView();
}

bool sqr_tools::CPieceUnitEditImp::IsPieceGroupChanged(void)
{
	if (MODEL)
	{
		CBaseModel::RenderPieceMap pPieceMap = MODEL->GetRenderPieces();

		if (pPieceMap.size() != m_LastPieceMap.size())
		{
			m_LastPieceMap = pPieceMap;
			return true;
		}

		CBaseModel::RenderPieceMap::iterator newItr = pPieceMap.begin();
		CBaseModel::RenderPieceMap::iterator oldItr = m_LastPieceMap.begin();
		while(newItr != pPieceMap.end())
		{
			if (newItr->first != oldItr->first)
			{
				m_LastPieceMap = pPieceMap;
				return true;
			}

			newItr++;
			oldItr++;
		}
	}

	return false;
}

void CPieceUnitEditImp::UpdateTreeView(void)
{
	if (MODEL)
	{
		if (!IsPieceGroupChanged())
			return;

		DeletePieces();

		CEditPieceGroup* pPieceGroup = NULL;
		pPieceGroup = new CEditPieceGroup;
		MODEL->GetEditPieceGroup(pPieceGroup);

		if (pPieceGroup)
		{
			for( unsigned int i = 0; i< pPieceGroup->GetPieceClassNum(); i++ )
			{
				CPieceClass& Pc = *pPieceGroup->GetPieceClass( i );
				for( unsigned int j = 0;j < Pc.size(); ++j )
				{
					CPieceUnitInfo PieceUnitInfo;
					PieceUnitInfo.name = Pc[j]->GetPieceName();
					AddPieceUnit(PieceUnitInfo, (CEditPiece*)Pc[j]);

					m_vtPieceUnitInfo.push_back(PieceUnitInfo);
				}
			}
		}

		pPieceGroup->Release();

		CCtrlBase::Update();
	}
}

void CPieceUnitEditImp::SetModel( CEditModel* pModel )
{
	m_pModel = pModel;
}

void CPieceUnitEditImp::AddPieceUnit(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece )
{
	AddPiece(PieceUnitInfo, Piece);
}


int CPieceUnitEditImp::AddPiece(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece )
{
	AddPieceRS(PieceUnitInfo, Piece);
	AddPieceVerAni(PieceUnitInfo, Piece);
	return 0;
}

int CPieceUnitEditImp::AddPieceRS(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece )
{
	PieceUnitInfo.RS_Names.clear();
	for( UINT i = 0; i < Piece->GetRSNum(); i++ )
	{
		CEditPieceRenderStyle* rs = (CEditPieceRenderStyle*)Piece->GetRS(i);
		std::string Str ;
		if(rs->GetEditRenderStyle() != NULL)
			Str = rs->GetEditRenderStyle()->GetName();
		else
		{
			std::string info;
			info = Piece->GetPieceName();
			string rsname = rs->GetRSName();
			if(rsname!="")
			{
				info += " 中没找到:";
				info += rsname.c_str();
			}
			else
				info += "这个piece中有没设置的RS";
			MessageBox(NULL,info.c_str(),"提示",MB_OK);
		}

		std::string RSIndexStr = rs->GetRSIndexName();
		if(Str.empty())
			Str = "Default";
		std::string tmp =Str + "("+RSIndexStr+")";
		Str = tmp;

		int index = Str.find_last_of("\\") + 1;
		Str = Str.substr(index);
		CPieceUnitInfo::STRVECTOR strVector;
		PieceUnitInfo.RS_Names.insert(CPieceUnitInfo::RSMAP::value_type(Str, strVector));

		for( int j = 0; j<rs->GetTextureNum(); j++ )
		{
			std::string TextName = rs->GetTexture( j )->GetName();
			index = TextName.find_last_of("\\") + 1;
			TextName = TextName.substr(index);
			PieceUnitInfo.RS_Names[Str].push_back(TextName);
		}
	}
	return 0;
}

int CPieceUnitEditImp::AddPieceVerAni(CPieceUnitInfo& PieceUnitInfo, CEditPiece* Piece )
{
	PieceUnitInfo.Ani_Names.clear();
	GVector<CMeshLevel>& MeshLevel = Piece->GetMesh()->GetMeshLevel();
	for( int Level = 0; Level < (int)MeshLevel.size(); Level++ )
	{
		std::string Str;
		char temp[256];
		sprintf_s(temp, "%g", MeshLevel[Level].GetDist());
		Str = temp;
		PieceUnitInfo.Ani_Names.push_back(Str);
	}
	return 0;
}

void CPieceUnitEditImp::DeletePieces()
{
	m_vtPieceUnitInfo.clear();
}

void sqr_tools::CPieceUnitEditImp::DeletePiece( std::string PieceName )
{
	if (MODEL)
	{
		PieceUnitInfoVector::iterator itr = m_vtPieceUnitInfo.begin();
		while(itr != m_vtPieceUnitInfo.end())
		{
			if (PieceName.compare((*itr).name) == 0)
			{
				MODEL->DeleteMeshPiece((*itr).name.c_str());
				m_vtPieceUnitInfo.erase(itr);
				break;
			} 
			else
				itr++;
		}
	}
}

void sqr_tools::CPieceUnitEditImp::CreateRS( std::string pieceName, std::string& rsName )
{
	if (MODEL)
	{
		CEditPieceGroup* pPieceGroup = NULL;
		pPieceGroup = new CEditPieceGroup;
		MODEL->GetEditPieceGroup(pPieceGroup);

		if (pPieceGroup)
		{
			for (unsigned int i = 0; i < pPieceGroup->GetPieceClassNum(); i++)
			{
				CPieceClass& Pc = *pPieceGroup->GetPieceClass(i);
				for( unsigned int j = 0; j < Pc.size(); ++j )
				{
					if (pieceName.compare(Pc[j]->GetPieceName()) != 0)
						continue;

					int maxIndex = 0;
					for (unsigned int k = 0; k < Pc[j]->GetRSNum(); k++)
					{
						std::string rsIndexName = Pc[j]->GetRS(k)->GetRSIndexName();
						if (rsIndexName.substr(0,4) != "默认")
							continue;

						int index;
						std::string strIndex = rsIndexName.substr(4);
						std::stringstream ss(strIndex);
						ss >> index;
						if (index >= maxIndex)
						{
							maxIndex = index + 1;
						}
					}

					std::string pieceResName;
					CPathSetImp::GetInst()->GetResFileName(rsName, pieceResName);
					CEditRenderStyle* pRs = new CEditRenderStyle( pieceResName.c_str(),L"res" );
					if( !pieceResName.empty() )
					{
						if(pRs->BinLoad()<0)
						{
							std::string str = pieceResName + "没载入成功";
							MessageBox(NULL, str.c_str(), "提示",MB_OK);
							return;
						}
					}

					char temp[256];
					sprintf_s(temp, "%i", maxIndex);
					std::string strIndexName = "默认" + string(temp);
					CEditPieceRenderStyle* rs = new CEditPieceRenderStyle((CEditPiece*)Pc[j], pRs);
					rs->SetRSIndexName(strIndexName);

					int index = rsName.find_last_of("\\") + 1;
					rsName = rsName.substr(index) + "(" + strIndexName + ")";
					return;
				}
			}
		}

		pPieceGroup->Release();
	}
}

void sqr_tools::CPieceUnitEditImp::DeleteRS( std::string pieceName, int rsIndex )
{
	if (MODEL)
	{
		CEditPieceGroup* pPieceGroup = NULL;
		pPieceGroup = new CEditPieceGroup;
		MODEL->GetEditPieceGroup(pPieceGroup);

		if (pPieceGroup)
		{
			for (unsigned int i = 0; i < pPieceGroup->GetPieceClassNum(); i++)
			{
				CPieceClass& Pc = *pPieceGroup->GetPieceClass(i);
				for( unsigned int j = 0; j < Pc.size(); ++j )
				{
					if (pieceName.compare(Pc[j]->GetPieceName()) != 0)
						continue;

					((CEditPiece*)Pc[j])->DeleteRS(rsIndex);
					return;
				}
			}
		}

		pPieceGroup->Release();
	}
}

void CPieceUnitEditImp::SelectPiece( std::string pieceName )
{
	if (MODEL)
		MODEL->SetSelectedPiece( pieceName );	
}

CPieceUnitEditImp::PieceUnitInfoVector& sqr_tools::CPieceUnitEditImp::GetPieceUnitInfo( void )
{
	return m_vtPieceUnitInfo;
}

void sqr_tools::CPieceUnitEditImp::SetRSIndexName( std::string pieceName, int rsIndex, std::string name )
{
	if (MODEL)
	{
		CEditPieceGroup* pPieceGroup = NULL;
		pPieceGroup = new CEditPieceGroup;
		MODEL->GetEditPieceGroup(pPieceGroup);

		if (pPieceGroup)
		{
			for (unsigned int i = 0; i < pPieceGroup->GetPieceClassNum(); i++)
			{
				CPieceClass& Pc = *pPieceGroup->GetPieceClass(i);
				for( unsigned int j = 0; j < Pc.size(); ++j )
				{
					if (pieceName.compare(Pc[j]->GetPieceName()) != 0)
						continue;

					CEditPieceRenderStyle* rs = (CEditPieceRenderStyle*)Pc[j]->GetRS(rsIndex);
					rs->SetRSIndexName(name);
					return;
				}
			}
		}

		pPieceGroup->Release();
	}
}

void sqr_tools::CPieceUnitEditImp::SelectChanged( std::string name )
{
	if (MODEL)
	{
		SelectPiece(name);

		CEditPiece* piece = MODEL->GetSelectPiece();

		MSG_SYS_SELCHANGE msg(NULL);
		m_pOwnContext->EventCollect(msg);
	}
}


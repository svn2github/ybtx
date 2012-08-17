#include "stdafx.h"
#include "CCtrlBase.h"
#include "CEditObserver.h"
#include "CEditContext.h"
#include "BoneBoxListCtrl/CBoneBoxListCtrlImp.h"
#include "BoneBoxListCtrl/CBoneBoxListDelegate.h"
#include "ContextMsg.h"
#include "COrientedBoundingBox.h"
#include "CVertexDeclaration.h"
#include "CEditModel.h"
#include "CEditMesh.h"

DEF_UI_IMP(CBoneBoxListCtrlImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

sqr::CBaseModel::RenderPieceMap  m_LastPieceMap1;

CBoneBoxListCtrlImp::CBoneBoxListCtrlImp( void* param )
:m_pDelegate(NULL)
{
	m_iItemNum = 0;
}

CBoneBoxListCtrlImp::~CBoneBoxListCtrlImp( void )
{
	if (m_pDelegate)
	{
		delete m_pDelegate;
		m_pDelegate = NULL;
	}
}

void CBoneBoxListCtrlImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);

	if (m_pDelegate)
		return;
	m_pDelegate = new CBoneBoxListDelegate(this);
	m_pOwnContext->RegisterDelegate(m_pDelegate);
	m_pOwnContext->ActiveDelegate(m_pDelegate);
}

void CBoneBoxListCtrlImp::ActiveNode( CRenderNode* pNode )
{
	if (MODEL && MODEL->GetSketetalFrame())
	{
		if (m_strLastSkeName != MODEL->GetSketetalFrame()->GetName() || IsPieceGroupChanged())
		{
			m_strLastSkeName = MODEL->GetSketetalFrame()->GetName();
			SetSelectedBoneName("");
			CCtrlBase::Update();
		}
		else
		{
			m_iItemNum = m_vtBoneNameList.size();

			UpdateList();

			if (m_iItemNum != m_vtBoneNameList.size())
			{
				m_iItemNum = m_vtBoneNameList.size();
				SetSelectedBoneName("");
				CCtrlBase::Update();
			}
		}
	}
	else
	{
		m_strLastSkeName = "";
		m_iItemNum = 0;
		SetSelectedBoneName("");
		CCtrlBase::Update();
	}
}

void sqr_tools::CBoneBoxListCtrlImp::UpdateList( void )
{
	m_vtBoneNameList.clear();

	if (MODEL && MODEL->GetSketetalFrame())
	{
		for( CModel::RenderPieceMap::iterator itP = MODEL->GetRenderPieces().begin(); itP != MODEL->GetRenderPieces().end(); itP++ )			
		{
			for( GVector<CRenderPiece::PieceData>::iterator it = itP->second.m_Pieces.begin(); it!= itP->second.m_Pieces.end(); ++ it )
			{
				if( (*it).m_pPiece && (*it).m_pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI &&
					string::npos == string((*it).m_pPiece->GetPieceName()).rfind("weapon") ) // 忽略武器的包围盒
				{
					CMesh* pMesh = (*it).m_pPiece->GetMesh();
					CMesh::BoxMap::iterator it = pMesh->m_BoxMap.begin();
					CMesh::BoxMap::iterator eit = pMesh->m_BoxMap.end();
					for(int i = 0;it!=eit;++it,++i)
					{
						string Name = MODEL->GetSketetalFrame()->GetSkeletalName(it->first);
						if (HasBone(Name) == -1)
							m_vtBoneNameList.push_back(Name);
					}	
				}
			}
		}
	}
}

void sqr_tools::CBoneBoxListCtrlImp::UpdateData( std::vector<std::string> str_vector )
{
	for (unsigned int i = 0; i < str_vector.size(); i++)
		AddSkeletalBox(str_vector[i]);

	CCtrlBase::Update();
}

int sqr_tools::CBoneBoxListCtrlImp::GetBoneNum( void )
{
	return m_vtBoneNameList.size();
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  

std::string sqr_tools::CBoneBoxListCtrlImp::GetBoneNameByIndex( int index )
{
	return m_vtBoneNameList[index];
}

void sqr_tools::CBoneBoxListCtrlImp::SetSelectedBoneName( std::string name )
{
	m_strSelBoneName = name;
	MSG_INFO info;
	info._handle = (void*)m_strSelBoneName.c_str();
	MSG_SYS_SELBONECHANGE msg(info);
	m_pOwnContext->EventCollect(msg);
}

int sqr_tools::CBoneBoxListCtrlImp::HasBone( std::string name )
{
	int index = -1;
	std::vector<std::string>::iterator itr = m_vtBoneNameList.begin();
	while (itr != m_vtBoneNameList.end())
	{
		++index;
		if (name.compare(*itr) == 0)
			return index;
		itr++;
	}
	return -1;
}

inline void MultPosMat( CVector3f& Out, const CVector3f& In, const CMatrix& mat )
{
	Out.x = In.x*mat._11 + In.y*mat._21 + In.z*mat._31 + mat._41;
	Out.y = In.x*mat._12 + In.y*mat._22 + In.z*mat._32 + mat._42;
	Out.z = In.x*mat._13 + In.y*mat._23 + In.z*mat._33 + mat._43;
}

int sqr_tools::CBoneBoxListCtrlImp::AddSkeletalBox( const string& SkName )
{
	if (!MODEL || !MODEL->GetSketetalFrame())
		return -1;

	int exsit = HasBone(SkName);
	int ID = MODEL->GetSkeletalIdByName(SkName.c_str());

	if(exsit==-1&& ID!=-1)
	{
		CMatrix TmpMat;
		CMatrix InvWorld;
		CMatrix Temp;

		MODEL->GetMatrix(TmpMat,ID,UINT(-1));
		MODEL->GetMatrix(InvWorld,0,UINT(-1));
		TmpMat.Invert();
		CEditSkeletalFrame* pFrame = (CEditSkeletalFrame*)MODEL->GetSketetalFrame();
		CVector3f TempPos;
		CVector3f Pos;
		CBaseAABB aabb;
		for( CModel::RenderPieceMap::iterator rit = MODEL->GetRenderPieces().begin(); rit != MODEL->GetRenderPieces().end(); rit++ )			
		{
			GVector<CRenderPiece::PieceData>::iterator it = rit->second.m_Pieces.begin();
			for( ; it != rit->second.m_Pieces.end(); ++it )
			{
				if( (*it).m_pPiece && (*it).m_pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI )
				{
					COrientedBoundingBox OBB;
					CAxisAlignedBox		AAB;

					CEditSkeletalMesh* pMesh = (CEditSkeletalMesh*)(*it).m_pPiece->GetMesh();
					GVector<VertexSkeletalFormat>& pVerSke = pMesh->m_OrgVertex;
					for(int i=0;i<(int)pVerSke.size();++i)
					{
						if( pVerSke[i].i.b[0] == ID ||
							pVerSke[i].i.b[1] == ID ||
							pVerSke[i].i.b[2] == ID ||
							pVerSke[i].i.b[3] == ID )
						{

							TempPos.x = 0.0f;
							TempPos.y = 0.0f;
							TempPos.z = 0.0f;
							for(int j = 0; j<4; j++ )
							{
								if( !pVerSke[i].i.b[j] )
								{
									if( j == 0 )
										MultPosMat( TempPos, *(CVector3f*)&(pVerSke[i].x), CMatrix::UNIT_MATRIX );
									break;
								}
								CSkeletalUnit* pSke = (*(CSkeletalFrame*)pFrame)[ int(pVerSke[i].i.b[j]) ];
								CMatrix temp;
								MODEL->GetMatrix(temp,UINT32(pVerSke[i].i.b[j]),UINT32(-1));
								temp = pSke->m_InverseMatrix.GetMATRIX()*temp;
								MultPosMat( Pos,  *(CVector3f*)&(pVerSke[i].x), temp );

								TempPos.x += Pos.x * pVerSke[i].w.GetWeight(j);
								TempPos.y += Pos.y * pVerSke[i].w.GetWeight(j);
								TempPos.z += Pos.z * pVerSke[i].w.GetWeight(j);
							}

							TempPos.FastMultiply(InvWorld);

							AAB.mergeBox(TempPos);
						}
					}

					if(!AAB.isNull())
					{
						AAB.transformBox(TmpMat);
						aabb.m_vMinimum = AAB.getMinimum();
						aabb.m_vMaximum	= AAB.getMaximum();
						pMesh->m_BoxMap.insert(CMesh::BoxMap::value_type(uint16(ID),aabb));
						MODEL->AddBox(ID,&pMesh->m_BoxMap[ID]);
					}
				}
			}
		}
		//g_Model.RefreshBox();
		m_vtBoneNameList.push_back(SkName);
		return m_vtBoneNameList.size() - 1;
	}	
	//g_Model.RefreshBox();
	return -1;
}

bool sqr_tools::CBoneBoxListCtrlImp::DelSkeletalBox( const string& SkName )
{
	if (MODEL)
	{
		int index = HasBone(SkName);
		m_vtBoneNameList.erase(m_vtBoneNameList.begin()+index);
		if(index!=-1)
		{
			int ID = MODEL->GetSkeletalIdByName(SkName.c_str());
			for( CModel::RenderPieceMap::iterator rit = MODEL->GetRenderPieces().begin(); rit != MODEL->GetRenderPieces().end(); rit++ )			
			{
				GVector<CRenderPiece::PieceData>::iterator it = rit->second.m_Pieces.begin();
				for( ; it != rit->second.m_Pieces.end(); ++it )
				{
					if( (*it).m_pPiece && (*it).m_pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI )
					{
						CMesh* pMesh = (*it).m_pPiece->GetMesh();
						MODEL->DelBox(&pMesh->m_BoxMap[ID]);
						pMesh->m_BoxMap.erase(uint16(ID));
					}
				}
			}
			return true;
		}
	}
	return false;
}

std::string sqr_tools::CBoneBoxListCtrlImp::AddPickBoneBox( void )
{
	if (MODEL)
	{
		if ( MODEL->GetCurSelSkelatal() == -1 )
		{
			MessageBox( NULL, "增加骨骼包围盒需要现在模型显示面板选中骨骼","错误", 0 );
			return "";
		}
		string BoneName = (*MODEL->GetSketetalFrame())[MODEL->GetCurSelSkelatal()]->m_pSkeletal->GetName();
		AddSkeletalBox(BoneName);
		return (*MODEL->GetSketetalFrame())[MODEL->GetCurSelSkelatal()]->m_pSkeletal->GetName();
	}
	return "";
}

bool sqr_tools::CBoneBoxListCtrlImp::FindItem( const string& SkName )
{
	if (MODEL && MODEL->GetSketetalFrame())
	{
		const CSkeletal& Ske = MODEL->GetSketetalFrame()->GetRootSkeletal();
		return -1!=Ske.GetSkeletalIDByName(SkName);
	}
	return false;
}

void sqr_tools::CBoneBoxListCtrlImp::LoadDefaultBone( void )
{
	if(FindItem("Bip01 Spine"))
	{
		AddSkeletalBox("Bip01 Spine");
	}

	if(FindItem("Bip01 Spine1"))
	{
		AddSkeletalBox("Bip01 Spine1");
	}

	if(FindItem("Bip01 R UpperArm"))
	{
		AddSkeletalBox("Bip01 R UpperArm");
	}

	if(FindItem("Bip01 L UpperArm"))
	{
		AddSkeletalBox("Bip01 L UpperArm");
	}

	if(FindItem("Bip01 R Thigh"))
	{
		AddSkeletalBox("Bip01 R Thigh");
	}

	if(FindItem("Bip01 L Thigh"))
	{
		AddSkeletalBox("Bip01 L Thigh");
	}

	if(FindItem("Bip01 R Forearm"))
	{
		AddSkeletalBox("Bip01 R Forearm");
	}

	if(FindItem("Bip01 L Forearm"))
	{
		AddSkeletalBox("Bip01 L Forearm");
	}

	if(FindItem("Bip01 R Calf"))
	{
		AddSkeletalBox("Bip01 R Calf");
	}

	if(FindItem("Bip01 L Calf"))
	{
		AddSkeletalBox("Bip01 L Calf");
	}
}

void sqr_tools::CBoneBoxListCtrlImp::AddHead( void )
{
	if(FindItem("Bip01 Head"))
		AddSkeletalBox("Bip01 Head");
}

bool sqr_tools::CBoneBoxListCtrlImp::IsPieceGroupChanged( void )
{
	if (MODEL)
	{
		CBaseModel::RenderPieceMap pPieceMap = MODEL->GetRenderPieces();

		if (pPieceMap.size() != m_LastPieceMap1.size())
		{
			m_LastPieceMap1 = pPieceMap;
			return true;
		}

		CBaseModel::RenderPieceMap::iterator newItr = pPieceMap.begin();
		CBaseModel::RenderPieceMap::iterator oldItr = m_LastPieceMap1.begin();
		while(newItr != pPieceMap.end())
		{
			if (newItr->first != oldItr->first)
			{
				m_LastPieceMap1 = pPieceMap;
				return true;
			}

			newItr++;
			oldItr++;
		}
	}

	return false;
}
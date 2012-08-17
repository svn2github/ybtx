#include "stdafx.h"
#include "CEditModel.h"
#include "CEffectPlayer.h"
#include "TIDPtrMapIter.h"
#include "TIDPtrMap.inl"
#include "TIDPtrMapIter.inl"
#include "CEditPieceGroup.h"
#include "CEditAnimationsRes.h"
#include "CRenderPipeline.h"
#include "BaseHelper.h"
#include "CIniFile.h"
#include "CGraphic.h"
#include "CEditorCore.h"
#include "CCamera.h"
#include "CodeCvs.h"
#include <fstream>
#include "TSqrAllocator.inl"
#include "CEditorConfig.h"
#include "CModCoder.h"
#include "CLight.h"
#include "CSkeletalsMgr.h"
#include "TIDPtrMapIter.h"
#include "TIDPtrMap.inl"
#include "TIDPtrMapIter.inl"
#include "CEditEffectProp.h"
#include "CEditEffect.h"

#define ENABLECP(VER)		0x00010000|VER	
#define ENABLEBOX(VER)		0x00100000|VER

#define GoldenSection		(float)0.61803398874989484820458683436564
#define tGShort				(float)(1 - GoldenSection) * 0.25f
WORD SkeletalIndex[]	= {
	0,	2,	3,
	0,	3,	4,
	0,	4,	5,
	0,	5,	2,
	1,	3,	2,
	1,	4,	3,
	1,	5,	4,
	1,	2,	5,
};

CVector3f SklPos[6] = { 
	CVector3f(0.0f,		0.0f,				0.0f),
	CVector3f(0.0f,		1.0f,				0.0f),
	CVector3f(tGShort,	1.0f - GoldenSection, 0.0f),
	CVector3f(0.0f,		1.0f - GoldenSection, tGShort),
	CVector3f(-tGShort,	1.0f - GoldenSection, 0.0f),
	CVector3f(0.0f,		1.0f - GoldenSection, -tGShort)
};

const DWORD  CEditModel::st_EditModelType = CRenderObject::st_RenderObjectType;
CEditModel::CEditModel(ILogicHandler* pHandler)
: CRenderModel(pHandler) 
, m_pSelectPiece(NULL)
{
	for(int i=1;i<=6;i++)
	{
		std::ostringstream os;
		os << "sf0" << i;
		m_KeyFilter.push_back(os.str());
	}
	for(int i=1;i<=5;i++)
	{
		std::ostringstream os;
		os << "cast0" << i;
		m_KeyFilter.push_back(os.str());
	}
	string key1 = "birth";
	m_KeyFilter.push_back(key1);

	m_SkeletalPos = NULL;
	m_pForceStyle = NULL;

	//////////////////////////////////////////////////////////////////////////
	//地编属性初始化
	m_bReceiveShadow		= false;
	m_bPlayAnimation		= false;
	m_bShadow				= true;
	m_bVisibleByConfigure	= false;
	m_bUseCameraCollision	= false;
	//////////////////////////////////////////////////////////////////////////
}

CEditModel::~CEditModel()
{

}

void CEditModel::SetSelectedPiece( const string& str )
{
	if(m_pSelectPiece)
	{
		m_pSelectPiece->SetForceRenderStyle(NULL);
		m_pSelectPiece = NULL;
	}

	ClearModelRender();
	RenderPieceMap::iterator it,eit = m_MeshsPiece.end();
	for( it = m_MeshsPiece.begin(); it!=eit; ++it )
	{
		for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
		{
			if( str == it->second.m_Pieces[i].m_pPiece->GetPieceName() )
			{
				m_pSelectPiece = (CEditPiece*)it->second.m_Pieces[i].m_pPiece;
				m_pSelectPiece->SetForceRenderStyle(&CEditorCore::TR_EPRS_SELECT);
				return;
			}
		}
	}
	DataChange();
	return;
}

int CEditModel::GetCurSelSkelatal()
{
	//return ((CEditSkeletalFrame*)m_pSkeFrame)->GetCurSelSkelatal();
	for( UINT i = 1; i < m_RenderMatrix.m_MaxUseSkeletalID; i++ )
	{
		if( m_SkeletalPos[i].m_IsSelected )
			return i;
	}
	return -1;
}

void CEditModel::GetEditPieceGroup(CEditPieceGroup* pPieceGroup)
{
	//pieces
	for ( RenderPieceMap::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); )
	{
		for ( int iPieces = 0; iPieces < (int)it->second.m_Pieces.size(); iPieces++ )
		{
			CPiece* pPiece = it->second.m_Pieces[iPieces].m_pPiece;
			pPiece->AddRef();
			if ( pPieceGroup->m_PieceClassIndex.find( pPiece->GetPieceClassName() ) == pPieceGroup->m_PieceClassIndex.end() )
			{
				int nIndex = pPieceGroup->m_PieceClassIndex.size();
				pPieceGroup->m_PieceClassIndex[ pPiece->GetPieceClassName() ] = nIndex;
				pPieceGroup->m_Piece.push_back( CPieceClass() );
			}
			pPieceGroup->m_Piece[ pPieceGroup->m_PieceClassIndex[ pPiece->GetPieceClassName() ] ].push_back( pPiece );
		}
		it++;
	}
	//头像
	pPieceGroup->m_pPortraitInfo = new PortraitInfo;
	*pPieceGroup->m_pPortraitInfo = m_PortraitInfo;
}

void CEditModel::AddMeshPieceEx( CEditPiece* pEpc, uint32 RSNum )
{
	m_Amalgamation = false;
	RenderPieceMap::iterator it = m_MeshsPiece.find( pEpc->GetPieceName() );
	pEpc->AddRef();
	if ( it != m_MeshsPiece.end() )
	{
		//for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
		//{
		//if( pEpc->GetPieceName() == it->second.m_Pieces[i].m_pPiece->GetPieceName() )
		//{
		if( pEpc != it->second.m_Pieces[0].m_pPiece)
		{
			SafeRelease( it->second.m_Pieces[0].m_pPiece );
			it->second.m_Pieces[0].m_pPiece = pEpc;
		}
		//return;
		//}
		//}
		//it->second.m_Pieces.push_back(CRenderPiece::PieceData());
		//CRenderPiece::PieceData& PD	= it->second.m_Pieces[ it->second.m_Pieces.size() - 1 ];
		//PD.m_pPiece         = pEpc;
		//PD.m_CurPiecesAniNo = 0;
		//PD.m_PrePiecesAniNo = 0;
		//return;
	}	
	m_MeshsPiece[  pEpc->GetPieceName() ]  = CRenderPiece( pEpc, (uint16)RSNum );
	if( pEpc->GetPieceGroup()->GetPortraitInfo() ) 
		SetPortraitInfo(*pEpc->GetPieceGroup()->GetPortraitInfo());

	SKELETAL_MASK SkeletalMask;
	_UpdateMesh(SkeletalMask);
	DataChange();
}

int CEditModel::GetSkeletalIdBySocketName( sqr::IDNAME SocketName, int* SocketNum )
{
	if (!m_pSkeFrame)
		return -1;

	return ((CEditSkeletalFrame*)m_pSkeFrame)->GetSkeletalIdBySocketName( SocketName,SocketNum );
}
float	CEditModel::GetCurFrameNum()
{
	int32 CurFrame = (int32)m_RenderMatrix.m_CurFrame;
	float Delta = m_RenderMatrix.m_CurFrame - CurFrame;
	
	uint32 uCurMaxFrame = uint32(GetCurMaxFrame());
	if ( uCurMaxFrame != 0 )
	{
		CurFrame %= uCurMaxFrame;
	}
	else
	{
		CurFrame = 0;
		// MessageBox(0, "此动画 max frame 为0", "", MB_OK);
	}
	
	return float(CurFrame) + Delta;
}

IEffect* CEditModel::GetFirstEffect()
{
	TIDPtrMap<sqr::EffectNode>::Iterator itr = m_pEfxMap.Begin();
	if (itr != m_pEfxMap.End())
		return (*itr).pEffect;
	else
		return NULL;
}

int CEditModel::GetCurMaxFrame(void)
{
	if( m_pSkeFrame && 0 != m_pAnimationGroup->GetAnimateNum())
		return m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate)->GetFrameNum();
	return -1;
}

const GMap<WORD,sqr::IDNAME>*	CEditModel::GetCurFrameString()
{
	if( m_pSkeFrame && 0 != m_pAnimationGroup->GetAnimateNum())
		return m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate)->GetFrameString();
	return NULL;
}

void CEditModel::InsertFrameString( const string& str, int FrameNum )
{
	if( m_pSkeFrame && 0 != m_pAnimationGroup->GetAnimateNum())
		((CEditAnimate*)m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate))->InsertFrameString(FrameNum,sqr::IDNAME(str.c_str()));
}

float	CEditModel::GetCurFrameRatio(void)
{
	if( m_pSkeFrame && 0 != m_pAnimationGroup->GetAnimateNum())
		return m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate)->GetFrameRatio();
	return 30.0f;
}

bool CEditModel::IsFindKeyAniName(string& aniname)
{
	GVector<string>::iterator result = find(m_KeyFilter.begin(),m_KeyFilter.end(),aniname);
	if(result==m_KeyFilter.end())
		return false;
	else
		return true;
}
bool CEditModel::ExportFrameKey( const string& szFileName )
{
	CAnimationGroup* pAniGroup = m_pAnimationGroup;
	if( pAniGroup == NULL )
		return false;

	CIniFile IniFile;
	IniFile.Create(gbk_to_utf16(szFileName).c_str());

	int     n			= pAniGroup->GetAnimateNum();
	string	aniname		= ""; 
	string  keyName		= "";
	WORD	nKeyFrame	= 0;
	WORD	nAttack01KeyFrame = 0;
	bool	attackFlag[4] = {false};
	bool	bIniWrited	= false;

	// 循环处理所有动画，特殊处理attack动画
	for( int i = 0; i < n; ++i )
	{
		aniname = (*pAniGroup)[i]->GetName();
		const GMap<WORD,sqr::IDNAME>& Keys = *(*pAniGroup)[i]->GetFrameString();
		size_t m = Keys.size();

		if (aniname.find("attack") != -1)
		{
			// 处理e字段之外的字段
			if (m != 0)
			{
				for(GMap<WORD,sqr::IDNAME>::const_iterator it = Keys.begin(); it != Keys.end(); it++)
				{
					nKeyFrame	= it->first;
					keyName		= it->second;
					if (keyName != "e")
					{
						IniFile.SetValue(aniname, keyName, nKeyFrame);
						bIniWrited = true;
					}
				}
			}

			// 为attack01 - attack04增加动画长度字段e
			keyName	  = "e";
			nKeyFrame = (*pAniGroup)[i]->GetFrameNum();
			if (aniname == "attack01")
			{
				attackFlag[0] = true;
				nAttack01KeyFrame = nKeyFrame;
			}
			else if (aniname == "attack02")
			{
				attackFlag[1] = true;
			}
			else if (aniname == "attack03")
			{
				attackFlag[2] = true;
			}
			else if (aniname == "attack04")
			{
				attackFlag[3] = true;
			}	
			IniFile.SetValue(aniname, keyName, nKeyFrame);
			bIniWrited = true;
		}
		else if(IsFindKeyAniName(aniname))
		{
			if (m != 0)
			{
				for(GMap<WORD,sqr::IDNAME>::const_iterator it = Keys.begin(); it != Keys.end(); it++)
				{
					nKeyFrame	= it->first;
					keyName		= it->second;
					if (keyName != "e")
					{
						IniFile.SetValue(aniname, keyName, nKeyFrame);
						bIniWrited = true;
					}
				}
			}
			//为sf01-sf06 cast01-cast05增加动画长度
			keyName	  = "e";
			nKeyFrame = (*pAniGroup)[i]->GetFrameNum();
			IniFile.SetValue(aniname, keyName, nKeyFrame);
			bIniWrited = true;
		}
		else
		{
			if (m != 0)
			{
				for(GMap<WORD,sqr::IDNAME>::const_iterator it = Keys.begin(); it != Keys.end(); it++)
				{
					nKeyFrame	= it->first;
					keyName		= it->second;
					IniFile.SetValue(aniname, keyName, nKeyFrame);
					bIniWrited = true;
				}
			}
			//为其他没有key的动作加上动作全帧长度
			keyName	  = "e";
			nKeyFrame = (*pAniGroup)[i]->GetFrameNum();
			IniFile.SetValue(aniname, keyName, nKeyFrame);
			bIniWrited = true;
		}
	}

	// 如果动作文件里包含attack动画，处理缺少的attack字段(共有最多4个attack动画)
	if (attackFlag[0])
	{
		for (int i=1; i<sizeof(attackFlag)/sizeof(attackFlag[0]) ; ++i)
		{
			if (!attackFlag[i])
			{
				std::ostringstream os;
				os << "attack0" << i+1;
				IniFile.SetValue(os.str(), "e", nAttack01KeyFrame);
				bIniWrited = true;
			}
		}
	}

	if (bIniWrited)
	{
		IniFile.Save(gbk_to_utf16(szFileName));
	}
	return true;
}

void CEditModel::ImportFrameKey( const string& szFileName )
{
	CPkgFile PkgFile;
	if( PkgFile.Open( PATH_ALIAS_EDITOR.c_str(), szFileName.c_str() ) != eFE_SUCCESS )
		return;

	uint16 szlength = 0;
	string aniname = "", keyName = "";
	WORD nKeyFrame = 0;
	int nAniCount = 0, nKeyCount = 0;
	GMap<string, GMap<int, string>> skeKeyInfo;

	///ani count
	PkgFile.read( &nAniCount, sizeof(nAniCount));
	for( int i = 0; i < nAniCount; ++i )
	{
		///ani name
		PkgFile.read(&szlength, sizeof(uint16));
		aniname.resize(szlength);
		PkgFile.read((TCHAR*)aniname.data(), szlength);
		GMap<int, string> keyInfo;

		///key count
		PkgFile.read( &nKeyCount, 4 );
		for( int i = 0; i < nKeyCount; ++i )
		{
			PkgFile.read( &nKeyFrame, sizeof(nKeyFrame));

			///key name
			PkgFile.read(&szlength, sizeof(uint16));
			keyName.resize(szlength);
			PkgFile.read((TCHAR*)keyName.data(), szlength);

			keyInfo[nKeyFrame] = keyName;
		}
		skeKeyInfo[aniname] = keyInfo;
	}
}

void CEditModel::PickVertex( PickSkeletalParam& Pick )
{
	return;
}

int CEditModel::PickSkeletal( PickSkeletalParam& Pick )
{
	if( !GetSketetalFrame()|| !GetAnimateGroup() )
		return GERROR;

	CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();


	CMatrix matView = pCamera->getViewMatrix();
	matView.InvertFast();
	CMatrix matProj = pCamera->getProjectionMatrix();

	CMatrix matViewProj = pCamera->getViewProjectMatrix();

	UINT i = 0,j = 0;
	for( i = 1; i < m_RenderMatrix.GetSkeletalCount(); i++ )
	{
		m_SkeletalPos[i].m_IsSelected = FALSE;

		CMatrix tScaleMat;
		CMatrix	tRotateMat;
		tRotateMat.SetRotateZ(-CMath::half_pi);
		tScaleMat.SetScale(CVector3f(m_SkeletalPos[i].m_Scale));

		if(!m_RenderMatrix.m_pMatrixUnit[i])
			continue;
		CMatrix matrix = m_RenderMatrix.m_pMatrixUnit[i]->GetMATRIX();

		CVector3f PickVer[6];
		CVector3f cur = CVector3f( (float)Pick.m_CurPos.x, (float)Pick.m_CurPos.y, 0);
		float z = FLT_MAX;
		int n1 = 0, n2 = 0;

		for( j = 0; j < 6; j++ )
		{
			PickVer[j] = SklPos[j];
			PickVer[j].FastMultiply(tScaleMat*tRotateMat*matrix );

			//z = min( PickVer[j].z, z );//求出这个点的zubffer以便于之后选择离屏幕最近的那根骨骼
		}

		CVector3f v;
		v.x = ( cur.x/Pick.m_Size.x - 1 ) / matProj._11;
		v.y = -( cur.y/Pick.m_Size.y - 1 ) / matProj._22;
		v.z = 1.0f;
		

		CVector3f vPickRayDir,vPickRayOrig;
		vPickRayDir.x = v.x*matView._11 + v.y*matView._21 + v.z*matView._31;
		vPickRayDir.y = v.x*matView._12 + v.y*matView._22 + v.z*matView._32;
		vPickRayDir.z = v.x*matView._13 + v.y*matView._23 + v.z*matView._33;
		vPickRayOrig.x = matView._41;
		vPickRayOrig.y = matView._42;
		vPickRayOrig.z = matView._43;

		float fu,fv,fd;
		BOOL IsPicked = FALSE;
		for( j = 0; j < 8; j++)
		{
			WORD* ind = SkeletalIndex + j*3;

			IsPicked = CMath::IntersectTri(PickVer[ind[0]],PickVer[ind[1]],PickVer[ind[2]],vPickRayOrig,vPickRayDir,fu,fv,fd);
			if(IsPicked)
				break;
		}

		if( /*z<Pick.m_Z&&*/IsPicked )
		{
			//Pick.m_Z = z;
			Pick.m_SkeletalID = i;
		}
	}

	if( Pick.m_SkeletalID )
	{
		m_SkeletalPos[ Pick.m_SkeletalID ].m_IsSelected = TRUE;
	}
	Pick.m_SkeletalName = GetSketetalFrame()->GetRootSkeletal().GetSkeletalNameByID( Pick.m_SkeletalID );
	DataChange();
	return 0;
}

int CEditModel::PickSocket( PickSocketParam& Pick )
{
	return -1;
}

int CEditModel::SaveJointVerNormal( const string& szFileName )
{
	return -1;
}

int CEditModel::LoadJointVerNormal( const string& szFileName )
{
	return -1;
}

//bool CEditModel::SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop, int32 DelayTime, float AniSpeed )
//{
//	bool ret = CModelEx::SetNextAnimation( AniName,FmPair,bLoop, DelayTime, AniSpeed );
//	return ret;
//}

const	AniResult CEditModel::RenderFrame( const CMatrix& matWorld, float uCurFrame, RenderObjStyle* pRORS) 
{
	UpdateBox();
	if (m_matCached!=matWorld)
	{
		m_matCached = matWorld;
		m_bNeedUpdate = false;
	}

	float DeltaFrame = uCurFrame - m_RenderMatrix.m_CurFrame;
	int32 DeltaTime = int32(DeltaFrame * GetCurFrameRatio());

	CalculateMatrix( DeltaTime );
	m_RenderMatrix.m_MatWorld = m_matCached;
	m_RenderMatrix.m_CurrentTime += DeltaTime;
	if (!m_isVisible)
		return m_RenderMatrix.m_Result;

	RenderObjStyle* pRenderRS = (pRORS && pRORS->m_eType == ROS_UNKNOWN) ? NULL : pRORS;

	if ( pRORS && m_eRenderStyle!=pRORS->m_eType)
		ClearModelRender();

	if (NULL != pRenderRS)
	{
		if (m_bNeedPushRender)
		{
			m_eRenderStyle = pRORS->m_eType;
			sMaterial* material = (sMaterial*)(pRORS->m_nParam2);
			if (NULL != material)
				m_Material = *(sMaterial*)(pRORS->m_nParam2);
			CGraphic::GetInst()->BeginModel(this);
			if ( pRORS->m_bRenderMesh )
				RenderMesh(  pRenderRS );
			CGraphic::GetInst()->EndModel();
			m_bNeedPushRender= false;
		}
		else
		{
			UpdateRenderStyle(pRenderRS);
		}
	}
	
	m_RenderMatrix.m_CurrentTime = max(1,m_RenderMatrix.m_CurrentTime);
	
	/*if ( !pRORS || pRORS->m_bRenderLink )
		RenderLink( m_RenderMatrix.m_CurrentTime, pRenderRS );*/

	if(m_OrgAABB.isNull())
		UpdateDynamicBox();

	//RenderDebugLine();

	return m_RenderMatrix.m_Result;
}

bool CEditModel::DecideAllIsSet()
{
	string strRSName = "";
	CModel::RenderPieceMap::iterator it,eit = m_MeshsPiece.end();
	for( it = m_MeshsPiece.begin(); it!=eit; ++it )
	{
		for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
		{
			if( !((CEditPiece*)it->second.m_Pieces[i].m_pPiece)->DecideIsSet() )
				return false;
		}
	}
	return true;
}

bool CEditModel::BinSave( const string& str )
{
	string FileName = str;
	if( !DecideAllIsSet() )
	{
		string log = FileName;
		log += "中有piece没有设置rs,或者rs为绝对路径,或者有纹理为绝对路径,不能保存";
		MessageBox( 0, log.c_str(), "提示",0 );
		return 0;
	}

	size_t npos = FileName.find(".mod");
	if( npos == -1 )
		FileName = FileName + ".mod";

	SetFileAttributes( FileName.c_str(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( FileName.c_str(), ios::binary|ios::out );
	locale::global(loc);
	if( File.fail() )
		return false;
	
	// zjtemp
#if 0
	CEditPieceGroup* pPieceGroup = GeneratePieceGroupForSave();
	CModFormat modFormat;
	modFormat.Save(FileName, pPieceGroup);
	pPieceGroup->Clear();
	SafeDelete(pPieceGroup);
#else
	//DWORD Res = MAKEFOURCC( 'A', 'R', 'P', 0 );
	DWORD Res = MAKEFOURCC( 'M', 'O', 'D', 0 );
	DWORD Version = ENABLECP(ARP_COMP_VERSION);
	Version = ENABLEBOX(Version);
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );

	int Size = 0;
	CModel::RenderPieceMap::iterator it,eit = m_MeshsPiece.end();

	for( it = m_MeshsPiece.begin(); it!=eit; ++it )
		Size += (int)it->second.m_Pieces.size();
	File.write( (TCHAR*)&Size, sizeof(int) );
	for( it = m_MeshsPiece.begin(); it!=eit; ++it )
	{
		for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
			((CEditPiece*)it->second.m_Pieces[i].m_pPiece)->BinSave( File );
	}

	// 头像信息
	File.write( (const char*)(&PortraitInfo::PICC),sizeof(FourCC) );
	File.write(reinterpret_cast<TCHAR*>(&m_PortraitInfo), sizeof(m_PortraitInfo));
	File.close();
#endif

	return true;
}

void CEditModel::SetSkesMask(const SKELETAL_MASK& mask)
{
	m_RenderMatrix.SetSkeletalMask(mask);
	DataChange();
}

void CEditModel::RenderBone(void)
{
	if( !GetSketetalFrame() || !GetAnimateGroup()||!GetAnimateGroup()->GetAniById( m_RenderMatrix.m_CurAnimate ))
		return;
	RenderParam	RP;
	//大小
	RP.m_RS.m_Material.Ambient = CColor4(0.5,0.5,0.5,1);//MOD
	RP.m_RS.m_Material.Diffuse	= CColor4::Green;//ADD
	RP.m_RS.m_LightEnable		= true;

	CMatrix tScaleMat;
	CMatrix	tRotateMat;

	CRenderPipeline::GetInst()->SetLight(0, CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem()->GetGlobalLight()->GetLightStruct());//ADD

	for( UINT i = 1; i < m_RenderMatrix.GetSkeletalCount(); i++ )
	{
		if(!m_RenderMatrix.m_pMatrixUnit[i])
			continue;
		tRotateMat.SetRotateZ(-CMath::half_pi);
		tScaleMat.SetScale(CVector3f(m_SkeletalPos[i].m_Scale.x,m_SkeletalPos[i].m_Scale.y,m_SkeletalPos[i].m_Scale.z));
		RP.m_Mat = tScaleMat*tRotateMat*m_RenderMatrix.m_pMatrixUnit[i]->GetMATRIX();
		if( m_SkeletalPos[i].m_IsSelected )
			RP.m_RS.m_Material.Diffuse	= CColor4::Red;//MOD
		else
			RP.m_RS.m_Material.Diffuse	= CColor4::Green;//MOD
		CRenderPipeline::GetInst()->Render( &CEditorCore::TP_RO_BONE, &RP );
	}
}

void CEditModel::RefreshBox(void)
{
	SKELETAL_MASK SkeletalMask;
	_UpdateMesh(SkeletalMask);
}

void CEditModel::DelBox(CBaseAABB* pBox)
{
	if(pBox==NULL)
		return;
	BoxPairPool::iterator iter = m_BoxMap.begin();
	while(iter!=m_BoxMap.end())
	{
		if(pBox == (*iter).pAABB)
		{
			m_BoxMap.erase(iter);
			return;
		}
		iter++;
	}
}

void CEditModel::AddBox(uint16 ID,CBaseAABB* pBox)
{
	m_BoxMap.push_back(BoxPair(ID,pBox));
}

void CEditModel::RenderBox(CBaseAABB* pCurBox,bool bStaticBox)
{
	CAxisAlignedBox aabb = m_OrgAABB;
	CMatrix m_CenterMatrix = GetCenterMatrix();
	aabb.transformBox(m_CenterMatrix);
	VerColor3D boxPoints[8];
	aabb.setExtents(aabb.getMinimum(),aabb.getMaximum());
	for (int i = 0; i < 8; i++)
	{
		boxPoints[i].p = CVector3f(aabb.getAllCorners()[i].x, aabb.getAllCorners()[i].y, aabb.getAllCorners()[i].z);
		boxPoints[i].diffuse = 0xff00ff00;
	}
	static VerColor3D linePoints[24];
	const TBYTE LINE_INDEX[24] = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 6, 1, 7, 2, 4, 3, 5 };
	for (int i = 0; i< 24; i++)
	{
		linePoints[i] = boxPoints[LINE_INDEX[i]];
	}
	CGraphic::GetInst()->DrawPrimitive(PT_LINELIST, linePoints, 24);

	//////////////////////////////////////////////////////////////////////////
	//draw static box
	if(bStaticBox)
	{
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
		{
			for ( size_t np = 0; np < it->second.m_Pieces.size(); ++np )
			{
				if ( it->second.m_Pieces[np].m_pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI 
					&& string("weapon") != it->second.m_Pieces[np].m_pPiece->GetPieceClassName() )
				{
					Ast(!it->second.m_Pieces[np].m_pPiece->GetAABox().isNull());
					m_OrgAABB.mergeBox(it->second.m_Pieces[np].m_pPiece->GetAABox());
					VerColor3D Box[24];
					CAxisAlignedBox psBox = it->second.m_Pieces[np].m_pPiece->GetAABox();
					CVector3f center = (psBox.m_vMinimum + psBox.m_vMaximum)/2;
					CVector3f extent = (psBox.m_vMaximum - psBox.m_vMinimum)/2;

					Box[0].p = (center + CVector3f( extent.x, extent.y, extent.z ));
					Box[1].p = (center + CVector3f( extent.x, extent.y, -extent.z ));
					Box[2].p = (center + CVector3f( extent.x, extent.y, extent.z ));
					Box[3].p = (center + CVector3f( extent.x, -extent.y, extent.z ));
					Box[4].p = (center + CVector3f( extent.x, -extent.y, -extent.z ));
					Box[5].p = (center + CVector3f( extent.x, extent.y, -extent.z ));
					Box[6].p = (center + CVector3f( extent.x, -extent.y, -extent.z ));
					Box[7].p = (center + CVector3f( extent.x, -extent.y, extent.z ));

					Box[8].p = (center + CVector3f( -extent.x, extent.y, extent.z ));
					Box[9].p = (center + CVector3f( -extent.x, extent.y, -extent.z ));
					Box[10].p = (center + CVector3f( -extent.x, extent.y, extent.z ));
					Box[11].p = (center + CVector3f( -extent.x, -extent.y, extent.z ));
					Box[12].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ));
					Box[13].p = (center + CVector3f( -extent.x, extent.y, -extent.z ));
					Box[14].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ));
					Box[15].p = (center + CVector3f( -extent.x, -extent.y, extent.z ));

					Box[16].p = (center + CVector3f( extent.x, extent.y, -extent.z ));
					Box[17].p = (center + CVector3f( -extent.x, extent.y, -extent.z ));
					Box[18].p = (center + CVector3f( extent.x, -extent.y, -extent.z ));
					Box[19].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ));
					Box[20].p = (center + CVector3f( extent.x, extent.y, extent.z ));
					Box[21].p = (center + CVector3f( -extent.x, extent.y, extent.z ));
					Box[22].p = (center + CVector3f( extent.x, -extent.y, extent.z ));
					Box[23].p = (center + CVector3f( -extent.x, -extent.y, extent.z ));

					Box[0].diffuse = Box[5].diffuse = Box[10].diffuse = Box[15].diffuse = Box[20].diffuse =
						Box[1].diffuse = Box[6].diffuse = Box[11].diffuse = Box[16].diffuse = Box[21].diffuse =
						Box[2].diffuse = Box[7].diffuse = Box[12].diffuse = Box[17].diffuse = Box[22].diffuse =
						Box[3].diffuse = Box[8].diffuse = Box[13].diffuse = Box[18].diffuse = Box[23].diffuse =
						Box[4].diffuse = Box[9].diffuse = Box[14].diffuse = Box[19].diffuse = pCurBox == &psBox?0xffffff00:0xff0000ff;
					//D3DXMatrixIdentity(&tmpMat);
					CGraphic::GetInst()->DrawPrimitive( PT_LINELIST,Box,24);
				}
			}
		}
	}
	else
	{
		for( CModel::RenderPieceMap::iterator rit = GetRenderPieces().begin(); rit != GetRenderPieces().end(); rit++ )			
		{
			GVector<CRenderPiece::PieceData>::iterator pit = rit->second.m_Pieces.begin();
			for( ; pit != rit->second.m_Pieces.end(); ++pit )
			{
				VerColor3D Box[24];
				CMesh::BoxMap::iterator it = (*pit).m_pPiece->GetMesh()->m_BoxMap.begin();
				CMesh::BoxMap::iterator eit =(*pit).m_pPiece->GetMesh()->m_BoxMap.end();
				CMatrix	TempMat;

				for (int i = 0;it!=eit;++it,++i)
				{
					CVector3f center = (it->second.m_vMinimum + it->second.m_vMaximum)/2;
					CVector3f extent = (it->second.m_vMaximum - it->second.m_vMinimum)/2;
					GetMatrix(TempMat,it->first,uint(-1));
					Box[0].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
					Box[1].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
					Box[2].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
					Box[3].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
					Box[4].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
					Box[5].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
					Box[6].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
					Box[7].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;

					Box[8].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
					Box[9].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
					Box[10].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
					Box[11].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;
					Box[12].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
					Box[13].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
					Box[14].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
					Box[15].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;

					Box[16].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
					Box[17].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
					Box[18].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
					Box[19].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
					Box[20].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
					Box[21].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
					Box[22].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
					Box[23].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;

					Box[0].diffuse = Box[5].diffuse = Box[10].diffuse = Box[15].diffuse = Box[20].diffuse =
						Box[1].diffuse = Box[6].diffuse = Box[11].diffuse = Box[16].diffuse = Box[21].diffuse =
						Box[2].diffuse = Box[7].diffuse = Box[12].diffuse = Box[17].diffuse = Box[22].diffuse =
						Box[3].diffuse = Box[8].diffuse = Box[13].diffuse = Box[18].diffuse = Box[23].diffuse =
						Box[4].diffuse = Box[9].diffuse = Box[14].diffuse = Box[19].diffuse = pCurBox == &it->second?0xffffff00:0xff0000ff;
					//D3DXMatrixIdentity(&tmpMat);
					CGraphic::GetInst()->DrawPrimitive( PT_LINELIST,Box,24);
				}
			}
		}
	}
}


int CEditModel::CalSkeletalSize(int nDelta)
{
	if( GetAnimateGroup() && GetSketetalFrame() && GetSketetalFrame()->GetSkeletalNum() == 0 )
		return 0;

	if(m_SkeletalPos)
		delete[] m_SkeletalPos;
	m_SkeletalPos = new SkeletalPos[ GetSketetalFrame()->GetSkeletalNum() ];

	CEditAnimate* Ani = (CEditAnimate*)GetAnimateGroup()->GetAniById( 0 );

	m_Size += m_Size*0.1f*nDelta;
	if( !nDelta )
	{
		m_Size = 0.0f;
		UINT count = m_RenderMatrix.GetSkeletalCount();
		for( UINT i = 0; i < count; i++ )
		{
			const CKeyFrames* KeyFrames = Ani->GetKeyFrame( i );
			if( KeyFrames->IsFrameExist() )
			{
				CQuaternion Quat;
				CVector3f Tran;
				KeyFrames->GetFirst(Quat,Tran,0.0f);
				float len = Tran.Mag();
				m_Size = max( m_Size, len);
				m_Size = min( 200, m_Size );
			}
		}
	}
	return CalAniSize( *(CEditSkeletal*)&GetSketetalFrame()->GetRootSkeletal(), *Ani, m_Size*3.0f );
}

int CEditModel::CalAniSize(const CEditSkeletal& Skeletal, CEditAnimate& Animate, float Size)
{
	int i,j,pos=0;
	CVector3f child[256];
	float len=0;
	BYTE SID = Skeletal.GetSkeletalID();
	if( SID && Skeletal.GetChildNum() )
	{
		for( i = 0; i < (int)Skeletal.GetChildNum(); i++ )
		{
			BYTE ID = Skeletal.GetChild(i)->GetSkeletalID();
			CQuaternion Quat;
			CVector3f Tran;
			Animate.GetKeyFrame( ID )->GetFirst(Quat,Tran,0.0f);
			CVector3f off = Tran;
			len = max( off.Mag(), len );
			for( j = 0; j < pos; j++ )
			{
				if( child[j] == off )
					break;
			}
			if( j == pos )
			{
				child[pos] = off;
				pos++;
			}
		}

		if( pos == 1 )//代表只有一个子骨骼
		{
			m_SkeletalPos[SID].m_Scale = sqr::CVector3f(Size*0.1f+child[0].y,child[0].x,Size*0.1f+child[0].z);
		}
		else
		{
			float p1[3]={ 0, 0, 0 };
			float p2[3]={ 0, 0, 0 };
			for( i=0; i<pos; i++ )
			{
				if( child[i].x>p2[0] )
					p2[0] = child[i].x;
				if( child[i].y>p2[1] )
					p2[1] = child[i].y;
				if( child[i].z>p2[2] )
					p2[2] = child[i].z;

				if( child[i].x<p1[0] )
					p1[0] = child[i].x;
				if( child[i].y<p1[1] )
					p1[1] = child[i].y;
				if( child[i].z<p1[2] )
					p1[2] = child[i].z;
			}
			float height = abs(p1[2]-p2[2]);
			float width = abs(p1[0]-p2[0]);
			float length = abs(p1[1]-p2[1]);
			m_SkeletalPos[SID].m_Scale = sqr::CVector3f(width,height,length);
		}
	}
	else
	{	
		float s = Size*0.1f;
		m_SkeletalPos[SID].m_Scale = sqr::CVector3f(s,4.0f,s);
	}

	//for( i = 0; i<6; i++ )
	//{
	//	m_SkeletalPos[SID].m_SklPos[i].np = m_SkeletalPos[SID].m_SklPos[i].p.Normalize();
	//}

	for( i = 0; i < (int)Skeletal.GetChildNum(); i++ )
		CalAniSize( *(CEditSkeletal*)Skeletal.GetChild(i), Animate, Size );
	return 0;
}

void CEditModel::SetPortraitInfo( const PortraitInfo& pi )
{
	m_PortraitInfo = pi;	
	DataChange();
}

void CEditModel::DeletePiece(CEditPiece* pEpc)
{
	RenderPieceMap::iterator it,eit = m_MeshsPiece.end();
	for( it = m_MeshsPiece.begin(); it!=eit; ++it )
	{
		bool isDel = true;
		for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
		{
			if( pEpc->GetPieceName() == it->second.m_Pieces[i].m_pPiece->GetPieceName() )
				SafeRelease( it->second.m_Pieces[i].m_pPiece );

			if(it->second.m_Pieces[i].m_pPiece != NULL)
				isDel = false;
		}
		if(isDel)
		{
			m_MeshsPiece.erase( it );
			break;
		}
	}
	ClearModelRender();
	DataChange();
}
//void CEditModel::pieceSelected( const CEditPiece* piece )
//{
//}

CEditPieceGroup* CEditModel::GeneratePieceGroupForSave()
{
	CEditPieceGroup*  pEditPieceGroup = new CEditPieceGroup;
	CPieceClass pc;
	RenderPieceMap::iterator it	 = m_MeshsPiece.begin();
	RenderPieceMap::iterator eit = m_MeshsPiece.end();
	for ( ; it != eit; ++it )
	{
		GVector<CRenderPiece::PieceData>::iterator rp_it	= it->second.m_Pieces.begin();
		GVector<CRenderPiece::PieceData>::iterator rp_eit	= it->second.m_Pieces.end();

		for ( ; rp_it != rp_eit; ++rp_it )
		{
			CPiece* piece = (*rp_it).m_pPiece;
			pc.push_back(piece);
		}
	}
	GVector<CPieceClass>& pPieceClass = pEditPieceGroup->GetPieceClass();
	pPieceClass.push_back(pc);

	pEditPieceGroup->SetPortraitInfo(GetPortraitInfo());

	return pEditPieceGroup;
}

void CEditModel::RemoveFrmParent(void)
{
	if(m_pParentSpace)
		m_pParentSpace->DelChild(this);
}

void CEditModel::ClearModelRender(void)
{
	if(m_pForceStyle)
	{
		NodePool::iterator it,eit = m_vRenderNode.end();
		for( it = m_vRenderNode.begin(); it!=eit; ++it)
		{
			if((*it).m_eType == ROS_SHADOW)
				continue;

			delete (*it).pRenderStyle;
		}
	}
	CRenderModel::ClearModelRender();
}

int	 CEditModel::RenderMesh(  RenderObjStyle* pRORS )
{
	int ret  = CRenderModel::RenderMesh(pRORS);
	if(m_pForceStyle)
	{
		NodePool::iterator it,eit = m_vRenderNode.end();
		for( it = m_vRenderNode.begin(); it!=eit; ++it)
		{			
			if((*it).m_eType == ROS_SHADOW)
				continue;

			CPieceRenderStyle* pTmpStyle = (*it).pRenderStyle;
			CEditPieceRenderStyle* pEditStyle = new CEditPieceRenderStyle;
			pEditStyle->SetRenderStyle(m_pForceStyle);
			(*it).pRenderStyle = pEditStyle;
		}
	}
	return ret;
}

void CEditModel::SetForceRenderStyle(CEditRenderStyle* pStyle)
{
	ClearModelRender();
	m_pForceStyle = pStyle;
}

void CEditModel::ClearMeshPiece()
{
	if(m_pSelectPiece)
	{
		m_pSelectPiece->SetForceRenderStyle(NULL);
		m_pSelectPiece = NULL;
	}
	
	CRenderModel::ClearMeshPiece();
}

CVector3f CEditModel::GetModelScale() const
{
	return m_vScale;
}

CVector3f sqr_tools::CEditModel::GetModelRotate() const
{
	CVector3f rotate;
	float x = m_Quaternion.x;
	float y = m_Quaternion.y;
	float z = m_Quaternion.z;
	float w = m_Quaternion.w;
	float sp = -2.0f * (y * z - w * x);

	if( fabs(sp) > 0.9999f )
	{
		rotate.x = 1.570796f * sp;
		rotate.y = CMath::ATan2(-x * z + w * y, 0.5f - y * y - z * z);
		rotate.z = 0.0f;
	}
	else
	{
		rotate.x = CMath::ASin(sp);
		rotate.y = CMath::ATan2(x * z + w * y, 0.5f - x * x - y * y);
		rotate.z = CMath::ATan2(x * y + w * z, 0.5f - x * x - z * z);
	}

	return rotate;
}

void sqr_tools::CEditModel::SetFrame( float uCurFrame )
{
	//float DeltaFrame = uCurFrame - m_RenderMatrix.m_CurFrame;
	//int32 DeltaTime = int32(DeltaFrame * GetCurFrameRatio());
	m_RenderMatrix.m_CurFrame = uCurFrame;
}

float sqr_tools::CEditModel::GetCurFrame( void )
{
	return m_RenderMatrix.m_CurFrame;
}

bool sqr_tools::CEditModel::AddPieceGroup( const char* szFileName )
{
	CPieceGroup* pPieceGroup = NULL;
	CGraphic* pGraphic = CGraphic::GetInst();
	if ( pGraphic->CreatePieceGroup(szFileName, &pPieceGroup) == -1 )
	{
		string log = szFileName;
		log = log + "模型没有找到，可能该文件不存在";

		MessageBox(NULL, log.c_str(), "错误", MB_OK);

		return false;
	}

	size_t nPieceClassNum = pPieceGroup->GetPieceClassNum();
	for ( size_t i = 0; i < nPieceClassNum; i++)
	{
		this->AddMeshPiece(pPieceGroup, pPieceGroup->GetPieceClassName(i), 0);
	}

	pPieceGroup->Release();
	m_strModelName = szFileName;

	return true;
}	

bool sqr_tools::CEditModel::AddAnimationGroup( const char* szFileName )
{
	ClearAnimateGroup();

	string sAgpFile = szFileName;
	sAgpFile = sAgpFile.substr(0,sAgpFile.size()-4)+".agp";
	CSkeletalFrame* pSke = NULL;
	CAnimationGroup* pAniGroup = NULL;
	if ( CGraphic::GetInst()->CreateAnimationGroup(sAgpFile.c_str(),&pAniGroup) == -1 )
	{
		string log = sAgpFile;
		log = log + "动画列表文件没有找到";

		MessageBox(NULL, log.c_str(), "错误", MB_OK);

		return false;
	}

	string sSkeFile = pAniGroup->GetSkeletalFrameName();
	if ( CSkeletalsMgr::GetInst()->GetSkeInPool(sSkeFile.c_str(),&pSke) == -1 )
	{
		string log = sSkeFile;
		log = log + "骨架文件没有找到";

		MessageBox(NULL, log.c_str(), "错误", MB_OK);

		return false;
	}

	SetSkeletalFrame(pSke);
	SetAnimateGroup(pAniGroup);

	string aniname = "stand01";
	if( pAniGroup )
	{
		string strAni = "";
		int nAniNum = pAniGroup->GetAnimationNum();
		for ( int i = 0; i < nAniNum; ++i )
		{
			aniname = pAniGroup->GetAnimationName(i);
			break;;
		}
	}
	SetNextAnimation(aniname.c_str(), FramePair(0,-1), true, 200, 1.0f);

	m_strAnimationName = szFileName;

	return true;
}

size_t sqr_tools::CEditModel::GetChildCount()
{
	return m_listNode.size();
}

std::string sqr_tools::CEditModel::GetModelName()
{
	return m_strModelName;
}

std::string sqr_tools::CEditModel::GetModelName(const size_t index)
{
	SpaceNodeList::iterator iter = m_listNode.begin();
	for ( size_t m = 0; m < index; ++m )
	{
		++iter;
	}
	CEditModel* pEditModel = (CEditModel*)(*iter);
	string strModelName;
	if( pEditModel )
		strModelName = pEditModel->GetModelName();

	return strModelName;
}

std::string sqr_tools::CEditModel::GetAnimationGroupName()
{
	return m_strAnimationName;
}

std::string sqr_tools::CEditModel::GetAnimationGroupName(const size_t index)
{
	SpaceNodeList::iterator iter = m_listNode.begin();
	for ( size_t m = 0; m < index; ++m )
	{
		++iter;
	}
	CEditModel* pEditModel = (CEditModel*)(*iter);
	string strAgpName;
	if( pEditModel )
		strAgpName = pEditModel->GetAnimationGroupName();

	return strAgpName;
}

std::string sqr_tools::CEditModel::GetEffectName(string& effectFileName)
{
	string effect;
	CEditEffect *pEffect = NULL;

	TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
	for( it = m_pEfxMap.Begin(); it!=eit; ++it )
	{
		pEffect = (CEditEffect*)(it->pEffect);
		if( pEffect )
		{
			effect = pEffect->GetFullEffectName();
			effectFileName = pEffect->GetEditEffectProp()->GetEffectGroup()->GetName();
		}
	}

	return effect;
}

std::string sqr_tools::CEditModel::GetEffectName(const size_t index, string& effectFileName)
{
	string effect;
	CEditEffect *pEffect = NULL;

	TIDPtrMap< EffectNode >::Iterator iter = m_pEfxMap.Begin();
	for( size_t i = 0; i < index; ++i )
		++iter;

	if( iter !=m_pEfxMap.End() )
	{
		pEffect = (CEditEffect*)(iter->pEffect);
		if( pEffect )
		{
			effect = pEffect->GetFullEffectName();
			effectFileName = pEffect->GetEditEffectProp()->GetEffectGroup()->GetName();
		}
	}

	return effect;
}

void sqr_tools::CEditModel::SetEditAnimateGroup( CAnimationGroup* AnimateGroup )
{
	m_strAnimationName = AnimateGroup->GetName();
	SetAnimateGroup(AnimateGroup);
}

std::string sqr_tools::CEditModel::GetAnimResName( void )
{
	if (m_RenderMatrix.m_Result.m_FrameString)
		return string(*m_RenderMatrix.m_Result.m_FrameString);
	else
		return "";
}

void sqr_tools::CEditModel::SetAnimResName( std::string name )
{
	if (m_RenderMatrix.m_Result.m_FrameString)
	{
		delete m_RenderMatrix.m_Result.m_FrameString;
		m_RenderMatrix.m_Result.m_FrameString = new IDNAME(name.c_str());
	}
	else
	{
		m_RenderMatrix.m_Result.m_FrameString = new IDNAME(name.c_str());
	}
}

bool sqr_tools::CEditModel::IsAnimate( void )
{
	return !m_RenderMatrix.m_nStopAni;
}

BYTE CEditModel::GetRenderStyleIndex()
{
	GMap< GString, CRenderPiece > &meshsPiece = this->GetRenderPieces();
	if ( meshsPiece.empty() ) 
		return 0;

	int nMaxRS = 0;
	for ( GMap< GString, CRenderPiece >::iterator it = meshsPiece.begin(); it != meshsPiece.end(); ++it )
	{
		CRenderPiece & RenderPiece = it->second;

		nMaxRS = max(nMaxRS, RenderPiece.m_RenderStyle);
	}

	return nMaxRS;
}
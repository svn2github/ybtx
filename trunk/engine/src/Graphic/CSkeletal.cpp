#include "stdafx.h"
#include "CSkeletal.h"
#include "CSkeletalFrame.h"
#include "CAnimationController.h"
#include "CRenderMatrix.h"
#include "CAnimation.h"
#include "Algorithm.h"
#include "TSqrAllocator.inl"

////////////////////////////////////////////////////////////////////////
// 功能：构造和析构
// 参数：略
// 返回：无
//////////////////////////////////////////////////////////////////////////
CSkeletal::CSkeletal()
{
	m_SkeletalID  = 0;
	m_Children.clear();
}

CSkeletal::~CSkeletal()
{
	Destroy();
}

void CSkeletal::Destroy()
{
	for ( int i=0; i<(int)m_Children.size(); i++ )
		delete m_Children[i];
	m_Children.clear();
}

//检查骨骼匹配
bool CSkeletal::IsFitForMe( const CSkeletal* Skeletal )
{
	if ( m_Children.size() == Skeletal->m_Children.size() )
	{
		for ( int i = 0; i< (int)m_Children.size(); i++ )
		{
			if ( !m_Children[i]->IsFitForMe( Skeletal->m_Children[i] ) )
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

//得到骨骼ID
int CSkeletal::GetSkeletalIDByName( const string& Name ) const
{
	int ID = -1;
	if ( m_SkeletalName == Name.c_str() )
		return m_SkeletalID;
	for ( int i = 0; i< (int)m_Children.size(); i++ )
	{
		ID = m_Children[i]->GetSkeletalIDByName( Name );
		if ( SUCCEEDED( ID ) )
			break;
	}
	return ID;
}

const char* CSkeletal::GetSkeletalNameByID( BYTE SkeletalID) const
{
	if( m_SkeletalID == SkeletalID )
		return m_SkeletalName.c_str();
	for( int i = 0; i< (int)m_Children.size(); i++ )
	{
		const char* Name = m_Children[i]->GetSkeletalNameByID( SkeletalID );
		if( Name != NULL )
			return Name;
	}
	return NULL;
}
void CSkeletal::CalculateMatrix( const CMatrixUnit& matUnit, CRenderMatrix& RenderMatrix,const CAnimationGroup& Animates,
					const CSkeletalFrame& SkeletalFrame, CAnimateSeparation* pParentSep ) const
{
	if ( m_SkeletalID > RenderMatrix.m_MaxUseSkeletalID ||
	RenderMatrix.m_pMatrixUnit.empty()||
	RenderMatrix.m_pMatrixUnit[ m_SkeletalID ]==NULL )
		return;

	CMatrixUnit& CurCMatrixUnit = *(RenderMatrix.m_pMatrixUnit[ m_SkeletalID ]);
	CMatrixUnit& CurCMatrixUnitOrg = *(RenderMatrix.m_pMatrixUnitOrg[ m_SkeletalID ]);
	CAnimateSeparation* pSeparation = RenderMatrix.m_pSkeletalSepList[m_SkeletalID];
	CAnimationController* pController = RenderMatrix.m_pAniCtrlList[m_SkeletalID];
	bool isAniBlend = RenderMatrix.m_IsAniBlend;
	float weight = RenderMatrix.m_Weight;
	WORD CurAnimateID = RenderMatrix.m_CurAnimate;
	float CurFrame = RenderMatrix.m_CurFrame;

	if( pSeparation==NULL )
		pSeparation = pParentSep;

	if( pSeparation!=NULL)
	{
		if( pSeparation->m_CurAnimate != CurAnimateID )
		{
			if(NULL!=Animates[pSeparation->m_CurAnimate] && Animates[pSeparation->m_CurAnimate]->IsValid())
			{
				CurAnimateID = pSeparation->m_CurAnimate;
				CurFrame	 = pSeparation->m_CurFrame;
				isAniBlend   = pSeparation->m_State != APS_NORMAL;
				weight		 = pSeparation->m_Weight;
			}
			else
				pSeparation = NULL; //资源尚未装在好
		}
		else if( pSeparation->m_State == APS_FADEOUT )
		{
			isAniBlend   = true;
			weight		 = pSeparation->m_Weight;
		}
	}

	//如果没有关键帧不计算
	if ( m_SkeletalID )
	{	
		float fParam = Animates[ CurAnimateID ]->GetTransParam();

		if ( !isAniBlend )//如果不需要动画间融合
		{
			const CKeyFrames* KeyFrameList = Animates[ CurAnimateID ]->GetKeyFrame( m_SkeletalID );

			if ( KeyFrameList != 0 && KeyFrameList->IsFrameExist() )
			{
				//本骨骼动画完成与否？
				WORD nMaxFrame = KeyFrameList->GetFrameNum();
				WORD wCurFrame = (WORD)CurFrame;
				//计算当前帧矩阵，保存
				if ( wCurFrame >= nMaxFrame )	//关键帧号
				{
					KeyFrameList->GetLast( CurCMatrixUnit.m_FrameQuat, CurCMatrixUnit.m_FrameTrans, fParam );
				}
				else
				{
					//在这个函数里面做了帧之间的插值了
					KeyFrameList->GetFrameData( CurCMatrixUnit.m_FrameQuat, CurCMatrixUnit.m_FrameTrans, CurFrame, fParam );
				}
				CurCMatrixUnitOrg = CurCMatrixUnit;
				MatrixUnitMultiply(CurCMatrixUnit,CurCMatrixUnit,matUnit);
			}
			else
			{
				CurCMatrixUnit = matUnit;
			}
		}
		else//如果需要动画间融合
		{
			const CKeyFrames* KeyFrameList =
				Animates[ CurAnimateID ]->GetKeyFrame( m_SkeletalID );

			if ( KeyFrameList !=0 && KeyFrameList->IsFrameExist() )
			{
				//得到融合时前后动画权重
				//计算当前帧矩阵，保存
				CMatrixUnit matTemp;

				if ( RenderMatrix.m_AniStartFrame == 0 )
					KeyFrameList->GetFirst( matTemp.m_FrameQuat, matTemp.m_FrameTrans, fParam );
				else
					KeyFrameList->GetFrameData( matTemp.m_FrameQuat, matTemp.m_FrameTrans, RenderMatrix.m_AniStartFrame, fParam );

				CMatrixUnit res;
				res.m_FrameTrans = CurCMatrixUnitOrg.m_FrameTrans*weight + matTemp.m_FrameTrans*( 1.0f - weight );
				res.m_FrameQuat.Slerp(CurCMatrixUnitOrg.m_FrameQuat,matTemp.m_FrameQuat,1.0f - weight);
				MatrixUnitMultiply(CurCMatrixUnit,res,matUnit);
			}
			else
			{
				CurCMatrixUnit = matUnit;
			}
		}
	}
	else
	{
		float fEndFrame = RenderMatrix.m_BlendFrame;
		if ( !RenderMatrix.m_IsAniBlend )//如果不需要动画间融合
		{
			const CKeyFrames* KeyFrameList = Animates[ CurAnimateID ]->GetKeyFrame( m_SkeletalID );
			if ( KeyFrameList != 0 && KeyFrameList->IsFrameExist() )
				fEndFrame = min( KeyFrameList->GetFrameNum(), RenderMatrix.m_AniEndFrame );
		}
		CurCMatrixUnit = matUnit;
	}

	if( NULL != pController)
		pController->DoControl( CurCMatrixUnit, matUnit, SkeletalFrame, Animates[ CurAnimateID ], CurFrame, RenderMatrix.m_CurrentTime );

	if ( abs( Animates[ CurAnimateID ]->GetScale() - 1.0f ) > 0.001f )
	{
		/*	CMatrix mat(
		Animates[ CurAnimateID ]->GetScale(), 0, 0, 0,
		0, Animates[ CurAnimateID ]->GetScale(), 0, 0,
		0, 0, Animates[ CurAnimateID ]->GetScale(), 0,
		0, 0, 0, 1 );*/
		CurCMatrixUnit.m_FrameTrans	*= Animates[ CurAnimateID ]->GetScale();
		CurCMatrixUnit.m_FrameQuat	*= Animates[ CurAnimateID ]->GetScale();
		//RenderMatrix.m_pMatrixUnit[ m_SkeletalID ]->m_CurCMatrixUnit =
		//	mat*RenderMatrix.m_pMatrixUnit[ m_SkeletalID ]->GetCMatrix();
	}

	//保存下来 GPU //
	if( RenderMatrix.m_pBoneMatrixlist.size() > m_SkeletalID )
	{
		MatrixUnitMultiply( RenderMatrix.m_pBoneMatrixlist[m_SkeletalID],
			SkeletalFrame[m_SkeletalID]->m_InverseMatrix , CurCMatrixUnit );

		RenderMatrix.m_pBoneMatrixlist[m_SkeletalID].UpCMatrixUnitToDualQuaternion();
	}

	//继续历遍子骨骼
	for ( int i=0; i<(int)m_Children.size(); i++ )
		m_Children[i]->CalculateMatrix( CurCMatrixUnit,
		RenderMatrix, Animates, SkeletalFrame ,pSeparation );
}

int CSkeletalUnit::BinLoad( CBufFile& GraphicFile )
{

	CMatrix tempMat;
	GraphicFile.SafeRead( &tempMat, sizeof( CMatrix ) );
	m_InverseMatrix = tempMat;
	int Size;
	GraphicFile.SafeRead( &Size, sizeof( Size ) );
	m_SocketList.resize( Size );
	if (Size>0)
		GraphicFile.SafeRead( &m_SocketList[0], sizeof( CSkeletalSocket )*Size );

	return 0;
}

int CSkeletalUnit::BinSave( WriteDataInf& wdi )
{
	int i;
	CMatrix tMatrix;
	m_InverseMatrix.GetMATRIX(tMatrix);
	wdi.Write( (TCHAR*)&tMatrix, sizeof( CMatrix ) );

	int Size = (int)m_SocketList.size();
	wdi.Write( (TCHAR*)&Size, sizeof( Size ) );
	for( i = 0; i < (int)m_SocketList.size(); i++ )
	{
		wdi.Write( m_SocketList[i].m_SocketName, sizeof( sqr::IDNAME ) );
		wdi.Write( (TCHAR*)&m_SocketList[i].m_Socket, sizeof( CMatrix ) );
	}

	return 0;
}


int CSkeletal::BinLoad( CBufFile& GraphicFile, BYTE ParentSkeletalID, CSkeletal** pSkeletal )
{
	m_ParentSkeletalID = ParentSkeletalID;

	int Size;
	LoadStringT( GraphicFile, m_SkeletalName );
	GraphicFile.SafeRead( &m_SkeletalID, sizeof(m_SkeletalID) );
	pSkeletal[m_SkeletalID] = this;

	GraphicFile.SafeRead( &Size, sizeof(Size) );
	m_Children.resize( Size, NULL );
	for ( int i = 0; i < (int)m_Children.size(); i++ )
	{
		m_Children[i] = new CSkeletal;
		m_Children[i]->BinLoad( GraphicFile, m_SkeletalID, pSkeletal );
	}

	return 0;
}

int CSkeletal::BinSave( WriteDataInf& wdi)
{
	int Size = (int)m_SkeletalName.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );
	wdi.Write( (TCHAR*)m_SkeletalName.data(), Size );
	wdi.Write( (TCHAR*)&m_SkeletalID, sizeof(m_SkeletalID) );

	Size = (int)m_Children.size();
	wdi.Write( (TCHAR*)&Size, sizeof(Size) );
	for( int i = 0; i < (int)m_Children.size(); i++ )
		((CSkeletal*)m_Children[i])->BinSave( wdi );

	return 0;
}
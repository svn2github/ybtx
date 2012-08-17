#include "stdafx.h"
#include "CAnimation.h"
#include "GraphicCommon.h"
#include "CTinyVector.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"

void GetBound( const GVector<uint16>& vecKey, uint16 nPos, uint16& nBegin, uint16& nEnd )
{
	nBegin = 0;
	nEnd = (uint16)vecKey.size() - 1;
	while ( nBegin != nEnd )
	{
		uint16 nCur = ( nBegin + nEnd )>>1;
		if ( nCur == nBegin )
			return;

		int n = nPos - vecKey[nCur];
		if ( n < 0 )
			nEnd = nCur;
		else
		{
			nBegin = nCur;
			if ( !n )
			{
				nEnd = nCur + 1;
				return;
			}
		}
	}
}

template<class node>
int GetBound( const GVector<node>& vecNode, int nVal )
{
	if ( vecNode.empty() ) return -1;
	int nBegin = 0;
	int nEnd = vecNode.size() - 1;

	for (;;)
	{
		if ( nBegin + 1 == nEnd )
		{
			return (nVal - vecNode[nBegin].key == 0)? nBegin:nEnd;
		}
		int nCur = ( nBegin + nEnd )>>1;
		int n = nVal - vecNode[nCur].key;
		if ( n == 0 ) return nCur;
		if ( n < 0 )
			nEnd = nCur;
		else
			nBegin = nCur;
	}
}

//void CAnimate::BinLoad()
//{
//	CreateResFile();
//	//DebugOut("%s,m_szName = %s",__FUNCTION__,m_szName.c_str());
//	//CPkgFile PkgFile;
//	//if ( PkgFile.Open( m_szName.c_str() ) != eFE_SUCCESS )
//	//	GenErr( ( GString( "不能识别的文件格式：" ) + m_szName ).c_str() );
//	//GVector<char> vecBuf( PkgFile.Size() );
//	//PkgFile.read( &vecBuf[0], vecBuf.size() );
//	//CBufFile GraphicFile( &vecBuf[0], vecBuf.size() );
//	//OnRead( GraphicFile );
//}

void CAnimate::OnRead( CRefBufFile& GraphicFile )
{
	if ( m_szName.empty() )
		return;

	uint32 Res;
	uint32 Version;
	GraphicFile.read( &Res, sizeof(uint32) );
	GraphicFile.read( &Version, sizeof(uint32) );

	if ( Res != MAKEFOURCC( 'A', 'N', 'I', 0 ) &&  Res != MAKEFOURCC( 'A', 'N', 'F', 0 ) )
		GenErr( GraphicErr::GetErrTypeStr(FileFormat_Err) );
	
	if(Version > ANF_VERSION || Version < ANF_VERSION - 3)
		GenErr( GraphicErr::GetErrTypeStr(FileVer_Err) );

	m_fScale = 1.0f;
	m_FrameRatio = 33.3f;
	GraphicFile.read( &m_MaxFrame, sizeof(m_MaxFrame) );

	GraphicFile.read( (char*)&m_FrameRatio, sizeof(m_FrameRatio) );
	GraphicFile.read( (char*)&m_fScale, sizeof(m_fScale) );
	GraphicFile.read( (char*)&m_fTransParam, sizeof(float) );
	if (Version>=10006)
		GraphicFile.read( (char*)&m_GroundSpeed, sizeof(float) );
	if (Version>=10008)
		GraphicFile.read( (char*)&m_IsCpAni,sizeof(bool));

	int Size;
	GraphicFile.read( &Size, sizeof(Size) );

	size_t kSize =  m_MaxFrame < 256 ? sizeof(uint8):sizeof(uint16);
	m_SkeletalList.resize( Size );
	if(m_IsCpAni)
	{
		for ( int i = 0; i < (int)m_SkeletalList.size(); i++ )
		{
			m_SkeletalList[i].BinLoad( GraphicFile ,m_fTransParam,kSize);
		}
	}
	else
	{
		for ( int i = 0; i < (int)m_SkeletalList.size(); i++ )
		{
			m_SkeletalList[i].BinLoadFull( GraphicFile , kSize);
		}
	}

	GraphicFile.read( &Size, sizeof(Size) );
	for ( int i = 0; i < Size; i++ )
	{
		WORD FrameNO;
		IDNAME Name;
		GraphicFile.read( ( &FrameNO ), sizeof( WORD ) );
		GraphicFile.read( ( &Name ), sizeof( IDNAME ) );
		m_FrameString[ FrameNO ] = Name;
	}

	if ( m_SkeletalList.empty() )
	{
		stringstream ss;
		ss << "动画骨骼列表为空 - 文件：" << GetRootGroup()->GetName() << "动画" << (const char*)m_AnimationeName;
		ArtErr( "动画文件本身存在错误", ss.str() );
	}

	m_szName = "";
	GetRootGroup()->LoadNextAni();
	Release();
}

CAnimate::CAnimate(CAnimationGroup* pAniGp, const string& szFileName, const wstring& Alias,IDNAME& AniName ) : CBaseGraphicRead( szFileName,Alias )
, CResUintNode(pAniGp)
, m_MaxFrame(0)
, m_FrameRatio(33.33f)
, m_fTransParam(0)
, m_AnimationeName( AniName )
, m_GroundSpeed( 0 )
, m_IsCpAni(true)
, m_bLoad(false)
{
}

int32	CAnimate::CreateResFile( void )
{
	m_bLoad = true;
	AddRef();
	int32 n =  CBaseGraphicRead::CreateResFile();
	if( n > 0 )
		m_pGroup->SetBufSize( m_pGroup->GetBufSize() + n );
	return n;
}

const IDNAME* CAnimate::GetFrameString( WORD CurFrameNum )
{
	if(IsValid())
	{
		GMap<WORD, IDNAME>::const_iterator it = m_FrameString.find( CurFrameNum );
		if (it != m_FrameString.end())
		{
			return &it->second;
		}
	}
	return NULL;
}

bool CAnimate::IsLoaded()
{
	return m_bLoad;
}

const GMap<WORD, GString> CAnimate::GetFootStepFrames()
{
	GMap<WORD, GString> mapFootStepFrames;
	if(IsValid())
	{
		GString strFrame("");
		GMap<WORD, IDNAME>::const_iterator beg = m_FrameString.begin();
		GMap<WORD, IDNAME>::const_iterator end = m_FrameString.end();
		for( beg; beg != end; ++beg )
		{
			strFrame = beg->second;
			if( strFrame.find("f") != -1 )//f == footstep,马蹄是设置的f1,f2,f3,f4
 				mapFootStepFrames[beg->first] = strFrame;
		}
	}

	return mapFootStepFrames;
}


void CKeyFrames::GetFrameData( CQuaternion& Quate, CVector3f& Trans, float fFrame, float fParam ) const
{
	uint16 uFrame = (uint16)fFrame;
	uint16 uBegin, uEnd;
	CVector3f TempTrans;
	CQuaternion TempQuat;
	GetBound( m_TranFrame, uFrame, uBegin, uEnd );
	if ( fFrame - m_TranFrame[uBegin] < 0.2f )
		Trans = m_TranData[uBegin]/*.Get( fParam )*/;
	else if ( m_TranFrame[uEnd] - fFrame < 0.2f )
		Trans = m_TranData[uEnd]/*.Get( fParam )*/;
	else
	{
		float wghTran = ( m_TranFrame[uEnd] - fFrame )/( m_TranFrame[uEnd] - m_TranFrame[uBegin] );
		Trans = m_TranData[uBegin]/*.Get( fParam )*/*wghTran + m_TranData[uEnd]/*.Get( fParam )*/*( 1.0f - wghTran );
	}

	GetBound( m_QuatFrame, uFrame, uBegin, uEnd );
	if ( fFrame - m_QuatFrame[uBegin] < 0.2f )
		Quate = m_QuatData[uBegin];
	else if ( m_QuatFrame[uEnd] - fFrame < 0.2f )
		Quate = m_QuatData[uEnd];
	else
	{
		float wgtQuat = ( m_QuatFrame[uEnd] - fFrame )/( m_QuatFrame[uEnd] - m_QuatFrame[uBegin] );
		CQuaternion Q1 = m_QuatData[uBegin];
		CQuaternion Q2 = m_QuatData[uEnd];
		CQuaternion::Slerp( Quate,Q1,Q2,1.0f - wgtQuat );
	}
}


void CKeyFrames::BinLoad( CBufFile& GraphicFile ,float fParam, size_t kSize)
{
	int TranFramNum;
	GraphicFile.SafeRead( &TranFramNum, sizeof(TranFramNum) );
	m_TranFrame.resize( TranFramNum );
	m_TranData.resize( TranFramNum );
	GVector<CTinyVector> vecTinyTranData;
	vecTinyTranData.resize( TranFramNum );
	for ( int i = 0; i < TranFramNum; i++ )
	{
		GraphicFile.SafeRead( (char*)&m_TranFrame[i], kSize );
		GraphicFile.SafeRead( (char*)&vecTinyTranData[i], sizeof(CTinyVector) );
		m_TranData[i] = vecTinyTranData[i].Get(fParam);
	}

	int QuatFramNum;
	GraphicFile.SafeRead( (char*)&QuatFramNum, sizeof(QuatFramNum) );
	m_QuatFrame.resize( QuatFramNum );
	m_QuatData.resize( QuatFramNum );
	GVector<CTinyQuaternion> vecTinyQuatData;
	vecTinyQuatData.resize( QuatFramNum );
	CTinyQuaternion TempTinyQuat;
	for ( int i = 0; i < QuatFramNum; i++ )
	{
		GraphicFile.SafeRead( (char*)&m_QuatFrame[i], kSize );
		GraphicFile.SafeRead( (char*)&vecTinyQuatData[i], sizeof(CTinyQuaternion) );
		m_QuatData[i] = vecTinyQuatData[i];
	}
	_RecordTinyData(vecTinyTranData,vecTinyQuatData);
	vecTinyTranData.clear();
	vecTinyQuatData.clear();
}


void CKeyFrames::BinLoadFull(CBufFile& GraphicFile, size_t kSize )
{
	int TranFramNum;
	GraphicFile.SafeRead( &TranFramNum, sizeof(TranFramNum) );
	m_TranFrame.resize( TranFramNum );
	m_TranData.resize( TranFramNum );
	for ( int i = 0; i < TranFramNum; i++ )
	{
		GraphicFile.SafeRead( (char*)&m_TranFrame[i], kSize );
		GraphicFile.SafeRead( (char*)&m_TranData[i], sizeof(CVector3f) );
	}

	int QuatFramNum;
	GraphicFile.SafeRead( (char*)&QuatFramNum, sizeof(QuatFramNum) );
	m_QuatFrame.resize( QuatFramNum );
	m_QuatData.resize( QuatFramNum );
	float tQuatdataFull[4];
	for ( int i = 0; i < QuatFramNum; i++ )
	{
		GraphicFile.SafeRead( (char*)&m_QuatFrame[i], kSize );
		GraphicFile.SafeRead( (char*)&tQuatdataFull, sizeof(float)*4 );
		m_QuatData[i].x = tQuatdataFull[0];
		m_QuatData[i].y = tQuatdataFull[1];
		m_QuatData[i].z = tQuatdataFull[2];
		m_QuatData[i].w = tQuatdataFull[3];
	}
}
//
//
//void CKeyFrames::BinSave( WriteDataInf& wdi ,float fParam)
//{
//	int TranFramNum = m_TranData.size();
//	wdi.Write( (TCHAR*)&TranFramNum, sizeof(TranFramNum) );
//
//	for(int i = 0;i<TranFramNum;i++)
//	{
//		wdi.Write( (TCHAR*)&m_TranFrame[i],sizeof(key) );
//	}
//
//	int QuatFramNum = m_QuatData.size();
//	wdi.Write( (TCHAR*)&QuatFramNum, sizeof(QuatFramNum) );
//	for(int i = 0;i<QuatFramNum;i++)
//	{
//		wdi.Write( (TCHAR*)&m_QuatFrame[i], sizeof(key) );
//	}
//}


//void CKeyFrames::BinSaveFull( WriteDataInf& wdi )
//{
//	int TranFramNum = m_TranData.size();
//	wdi.Write( (TCHAR*)&TranFramNum, sizeof(TranFramNum) );
//	for( int i = 0;i<TranFramNum;i++)
//	{
//		wdi.Write( (TCHAR*)&m_TranFrame[i],sizeof(key) );
//		wdi.Write( (TCHAR*)&m_TranData[i], sizeof(CVector3f) );
//	}
//
//	int QuatFramNum = m_QuatData.size();
//	wdi.Write( (TCHAR*)&QuatFramNum, sizeof(QuatFramNum) );
//	float tQuatdataFull[4];
//	for(int i = 0;i<QuatFramNum;i++)
//	{
//		wdi.Write( (TCHAR*)&m_QuatFrame[i], sizeof(key) );
//		tQuatdataFull[0] = m_QuatData[i].x ;
//		tQuatdataFull[1] = m_QuatData[i].y ;
//		tQuatdataFull[2] = m_QuatData[i].z ;
//		tQuatdataFull[3] = m_QuatData[i].w ;
//		wdi.Write( (TCHAR*)&tQuatdataFull, sizeof(float)*4 );
//	}
//}
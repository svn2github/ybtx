#include "stdafx.h"
#include "CEditAnimate.h"
#include "StringHelper.h"
#include "GraphicCommon.h"
#include <fstream>
void	CEditAnimate::InsertFrameString( WORD Frame, sqr::IDNAME& Name )
{
	if( Frame<m_MaxFrame )
	{
		if( ( (LPCTSTR)Name )[0] )
			m_FrameString[ Frame ] = Name; 
		else
			m_FrameString.erase( Frame );
	}
	else
	{
		string ErrMsg = format("设置的当前帧：%d 不小于该动作最大帧：%d，请重设", Frame, m_MaxFrame);
		MessageBox( 0, ErrMsg.c_str(), "提示", 0 );
	}
}

int CEditAnimate::BinSave( string SaveName )
{
	SetFileAttributes( SaveName.data(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( SaveName.data(), ios::binary|ios::out );
	locale::global(loc);

	DWORD Res = MAKEFOURCC( 'A', 'N', 'I', 0 );
	DWORD Version = ANF_VERSION;
	m_IsCpAni = false;
	File.write( (TCHAR*)&Res, sizeof(DWORD) );
	File.write( (TCHAR*)&Version, sizeof(DWORD) );
	File.write( (TCHAR*)&m_MaxFrame, sizeof(m_MaxFrame) );
	File.write( (TCHAR*)&m_FrameRatio, sizeof(m_FrameRatio) );
	File.write( (TCHAR*)&m_fScale, sizeof(m_fScale) );
	File.write( (TCHAR*)&m_fTransParam, sizeof(float) );
	File.write(	(TCHAR*)&m_GroundSpeed, sizeof(float) );
	File.write( (TCHAR*)&m_IsCpAni,sizeof(bool));

	int Size = (int)m_SkeletalList.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	int i;
	//if (m_IsCpAni)
	//{
	//	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
	//	{
	//		if ( m_MaxFrame < 256 )
	//			((TEditKeyFrames<uint8>*)m_SkeletalList[i])->BinSave( File ,m_fTransParam);
	//		else
	//			((TEditKeyFrames<uint16>*)m_SkeletalList[i])->BinSave( File ,m_fTransParam);
	//	}
	//}
	//else
	//{
	size_t kSize =  m_MaxFrame < 256 ? sizeof(uint8):sizeof(uint16);
	for( i = 0; i < (int)m_SkeletalList.size(); i++ )
	{

		//if ( m_MaxFrame < 256 )
		//	((TEditKeyFrames<uint8>*)m_SkeletalList[i])->BinSaveFull( File );
		//else
		//	((TEditKeyFrames<uint16>*)m_SkeletalList[i])->BinSaveFull( File );
		static_cast<TEditKeyFrames*>(&m_SkeletalList[i])->BinSaveFull( File, kSize );
	}
	//}

	Size = (int)m_FrameString.size();
	File.write( (TCHAR*)&Size, sizeof(Size) );
	for( GMap<WORD,sqr::IDNAME>::iterator it = m_FrameString.begin(); it != m_FrameString.end(); ++it, i++ )
	{
		File.write( (TCHAR*)( &(*it).first ), sizeof( WORD ) );
		File.write( (TCHAR*)( &(*it).second ), sizeof( sqr::IDNAME ) );
	}

	return 0;
}

//template<class key>
//void TEditKeyFrames<key>::BinSave( ofstream& File ,float fParam)
//{
//	int TranFramNum = m_TranData.size();
//	File.write( (TCHAR*)&TranFramNum, sizeof(TranFramNum) );
//	
//	for(int i = 0;i<TranFramNum;i++)
//	{
//		File.write( (TCHAR*)&m_TranFrame[i],sizeof(key) );
//	}
//
//	int QuatFramNum = m_QuatData.size();
//	File.write( (TCHAR*)&QuatFramNum, sizeof(QuatFramNum) );
//	for(int i = 0;i<QuatFramNum;i++)
//	{
//		File.write( (TCHAR*)&m_QuatFrame[i], sizeof(key) );
//	}
//}

void TEditKeyFrames::BinSaveFull(ofstream& File, size_t kSize)
{
	int TranFramNum = m_TranData.size();
	File.write( (TCHAR*)&TranFramNum, sizeof(TranFramNum) );
	for( int i = 0;i<TranFramNum;i++)
	{
		File.write( (TCHAR*)&m_TranFrame[i], kSize );
		File.write( (TCHAR*)&m_TranData[i], sizeof(CVector3f) );
	}

	int QuatFramNum = m_QuatData.size();
	File.write( (TCHAR*)&QuatFramNum, sizeof(QuatFramNum) );
	float tQuatdataFull[4];
	for(int i = 0;i<QuatFramNum;i++)
	{
		File.write( (TCHAR*)&m_QuatFrame[i], kSize );
		tQuatdataFull[0] = m_QuatData[i].x ;
		tQuatdataFull[1] = m_QuatData[i].y ;
		tQuatdataFull[2] = m_QuatData[i].z ;
		tQuatdataFull[3] = m_QuatData[i].w ;
		File.write( (TCHAR*)&tQuatdataFull, sizeof(float)*4 );
	}
}
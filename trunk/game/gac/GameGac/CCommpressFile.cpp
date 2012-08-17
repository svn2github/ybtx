#include "stdafx.h"
#include "CCommpressFile.h"
#include "BaseHelper.h"

#define SCENE_UNIT_TEXTRUE	""

void CCommpressFile::CleanData()
{ 
	m_CommpressData.clear(); 

	m_nWidth  = 0;
	m_nHeight = 0;
}

bool CCommpressFile::ReadFile( const TCHAR* szInFile, uint8 uSceneID, IGraphic* pGraphic  )
{
	CleanData();
	CPkgFile inFile( L"", szInFile, true );

	if(!inFile)
		return false;

	//判断输入是否acf文件
	tagACFHeader acfHeader;
	inFile.Read( (TCHAR*)&acfHeader, sizeof(tagACFHeader) );
	if( acfHeader.dwFlag != MakeDWORD( 'A', 'C', 'F', ' ' ) )
		return false;

	m_nWidth  = acfHeader.dwWidth;
	m_nHeight = acfHeader.dwHeight;

	//分配内存空间
	m_CommpressData.resize( m_nWidth * m_nHeight / 2 );
	TCHAR* lpData = &m_CommpressData[0];

	int16 countData;
	__int64 data;
	uint32 nCount = 0;
	while( inFile.Read( (TCHAR*)&countData, sizeof(countData) ) )
	{
		if( countData > 0 )
		{
			inFile.Read( (TCHAR*)&data, sizeof(data) );
			for( int i = 0; i < countData; i++ )
			{				
				memcpy( lpData + nCount, (void*)&data, sizeof(data) );
				nCount += sizeof(data);
			}

		}
		else if ( countData < 0 )
		{

			for( int i = 0; i < -countData; i++ )
			{		
				inFile.Read( (TCHAR*)&data, sizeof(data) );
				memcpy( lpData + nCount, (void*)&data, sizeof(data) );
				nCount += sizeof(data);
			}
		}
	}

	Ast( nCount == m_CommpressData.size() );

	return true;
}
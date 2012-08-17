// GLobD3d.cpp: implementation of the GLobD3d class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLobD3d.h"

//////////////////////////////////////////////////////////////////

extern int g_XCur=0;          //全局鼠标X分量
extern int g_YCur=0;          //全局鼠标Y分量

extern int g_XScreen=0;       //全局屏幕X大小
extern int g_YScreen=0;       //全局屏幕X大小

//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
_CovData _CovData::_s;

void FillVecData( D3DXVECTOR3* pVec, uint32 nYaw, uint32* pIndStr, uint32* pIndNum )
{
	const double fDelSin_V = sin( MATH_PI*0.5/nYaw );
	const double fDelCos_V = cos( MATH_PI*0.5/nYaw );

	double fCurSin = 0.0f;
	double fCurCos = 1.0f;
	for( uint32 i = 0, k = 0; i < nYaw + 1; i++ )
	{
		uint32 m = (uint32)( nYaw*fCurSin );
		pIndStr[i] = k;
		pIndNum[i] = m + 1;

		const double fDelSin_i = m ? sin( MATH_PI*0.5/m ) : 0.0f;
		const double fDelCos_i = m ? cos( MATH_PI*0.5/m ) : 1.0f;

		double fCurSin_i = 0.0f;
		double fCurCos_i = 1.0f;
		for( uint32 j = 0; j < m + 1; j++, k++ )
		{
			pVec[k].x = (float)( fCurCos_i*fCurSin );
			pVec[k].y = (float)( fCurCos );
			pVec[k].z = (float)( fCurSin_i*fCurSin );

			double sinf_j = fCurSin_i*fDelCos_i + fCurCos_i*fDelSin_i;
			double cosf_j = fCurCos_i*fDelCos_i - fCurSin_i*fDelSin_i;
			fCurSin_i = sinf_j;
			fCurCos_i = max( cosf_j, 0 );
		}

		double sinf_i = fCurSin*fDelCos_V + fCurCos*fDelSin_V;
		double cosf_i = fCurCos*fDelCos_V - fCurSin*fDelSin_V;
		fCurSin = sinf_i;
		fCurCos = max( cosf_i, 0 );
	}
}

_CovData::_CovData()
{
	// 初始化四元数压缩的矢量部分数据
	FillVecData( m_fVecInd, eCD_MAX_YAW, m_uIndStr, m_uIndNum );

	for( int n = 0, i = 0; n < 8; n++, i += eCD_NOR_BUF )
	{
		FillVecData( m_fNorInd + i, eCD_NOR_YAW, m_uNorStr, m_uNorNum );
		D3DXVECTOR3 vecSG( n&eCD_NOR_SGX ? -1 : 1, n&eCD_NOR_SGY ? -1 : 1, n&eCD_NOR_SGZ ? -1 : 1 );
		for( int j = 0; j < eCD_NOR_BUF; j++ )
		{
			m_fNorInd[ i + j ].x *= vecSG.x;
			m_fNorInd[ i + j ].y *= vecSG.y;
			m_fNorInd[ i + j ].z *= vecSG.z;
		}
	}

	// 初始化四元数压缩的标量部分数据
	const double fDelSin_W = sin( MATH_PI/eCD_WGT_MSK );
	const double fDelCos_W = cos( MATH_PI/eCD_WGT_MSK );
	double fCurSin = 0.0f;
	double fCurCos = 1.0f;
	for( int i = 0; i < eCD_MAX_PTC; i++ )
	{
		m_fWgtCos[i] = (float)fCurCos;
		m_fWgtSin[i] = (float)fCurSin;

		double sinf_i = fCurSin*fDelCos_W + fCurCos*fDelSin_W;
		double cosf_i = fCurCos*fDelCos_W - fCurSin*fDelSin_W;
		fCurSin = sinf_i;
		fCurCos = cosf_i;
	}
}
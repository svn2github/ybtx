#include "stdafx.h"
#include "CTinyVector.h"
#include "TSqrAllocator.inl"

namespace sqr
{

_CovData _CovData::_s;

_CovData::_CovData()
{
	// 初始化四元数压缩的矢量部分数据
	DivSphSurface( (CVector3f*)m_fVecInd, eCD_MAX_YAW, m_uIndStr, m_uIndNum );

	for ( int n = 0, i = 0; n < 8; n++, i += eCD_NOR_BUF )
	{
		DivSphSurface( (CVector3f*)( m_fNorInd + i ), eCD_NOR_YAW, m_uNorStr, m_uNorNum );
		CVector3f vecSG( n&eCD_NOR_SGX ? -1.0f : 1.0f, n&eCD_NOR_SGY ? -1.0f : 1.0f, n&eCD_NOR_SGZ ? -1.0f : 1.0f );
		for ( int j = 0; j < eCD_NOR_BUF; j++ )
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
	for ( int i = 0; i < eCD_MAX_PTC; i++ )
	{
		m_fWgtCos[i] = (float)fCurCos;
		m_fWgtSin[i] = (float)fCurSin;

		double sinf_i = fCurSin*fDelCos_W + fCurCos*fDelSin_W;
		double cosf_i = fCurCos*fDelCos_W - fCurSin*fDelSin_W;
		fCurSin = sinf_i;
		fCurCos = cosf_i;
	}
}

}


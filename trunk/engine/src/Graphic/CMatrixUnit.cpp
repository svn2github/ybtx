#include "stdafx.h"
#include "CMatrixUnit.h"

const CMatrixUnit CMatrixUnit::CMatrixUnitZero(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f);


CMatrixUnit& CMatrixUnit::operator = ( const CMatrixUnit& fUnit )
{
	m_FrameTrans	= fUnit.m_FrameTrans;
	m_FrameQuat		= fUnit.m_FrameQuat;
	m_FrameFloat	= fUnit.m_FrameFloat;
	return *this;
}

CMatrixUnit& CMatrixUnit::operator = (const CMatrix& fMat)
{
	m_FrameQuat.FromRotationMatrix(fMat);
	m_FrameTrans.x = fMat.m[3][0] ;
	m_FrameTrans.y = fMat.m[3][1] ;
	m_FrameTrans.z = fMat.m[3][2] ;
	m_FrameFloat   = 0.0f;
	return *this;
}

CMatrixUnit& CMatrixUnit::operator *= ( const CMatrixUnit& fUnit )
{
	m_FrameQuat  *= fUnit.m_FrameQuat;
	m_FrameTrans =	fUnit.m_FrameQuat*m_FrameTrans;
	m_FrameTrans += fUnit.m_FrameTrans;
	return *this;
}

CMatrixUnit& CMatrixUnit::operator *= ( const float fR )
{
#ifdef SIMD
	__asm
	{
		mov esi,this
		movups xmm0,[esi]
		movups xmm1,[esi + 16]

		movss  xmm2,fR
		shufps xmm2,xmm2,0x0

		mulps xmm0,xmm2
		mulps xmm1,xmm2

		movups [esi],xmm0
		movups [esi + 16],xmm1
	}
#else
	Quat[0] *= fR;
	Quat[1] *= fR;
	Quat[2] *= fR;
	Quat[3] *= fR;
	Tran[0] *= fR;
	Tran[1] *= fR;
	Tran[2] *= fR;
	Tran[3] *= fR;	
#endif
	return *this;
}

CMatrixUnit	CMatrixUnit::operator * ( const float fR )
{
	__declspec(align(16)) CMatrixUnit tUnit;
#ifdef SIMD
	__asm
	{
		mov esi,this
		movups xmm0,[esi]
		movups xmm1,[esi + 16]

		movss  xmm2,fR
		shufps xmm2,xmm2,0x0

		mulps xmm0,xmm2
		mulps xmm1,xmm2

		movaps [tUnit],xmm0
		movaps [tUnit + 16],xmm1
	}
#else
	tUnit.Quat[0]  = Quat[0] * fR;
	tUnit.Quat[1]  = Quat[1] * fR;
	tUnit.Quat[2]  = Quat[2] * fR;
	tUnit.Quat[3]  = Quat[3] * fR;
	tUnit.Tran[0]  = Tran[0] * fR;
	tUnit.Tran[1]  = Tran[1] * fR;
	tUnit.Tran[2]  = Tran[2] * fR;
	tUnit.Tran[3]  = Tran[3] * fR;	
#endif
	return tUnit;
}

CMatrixUnit& CMatrixUnit::operator += ( const CMatrixUnit& fUnit )
{
#ifdef SIMD
	__asm
	{
		mov esi,this
		mov edi,fUnit
		movups xmm0,[esi]
		movups xmm1,[edi]
		addps xmm0,xmm1
		movups [esi],xmm0

		movups xmm0,[esi + 16]
		movups xmm1,[edi + 16]
		addps xmm0,xmm1
		movups [esi + 16],xmm0
	}
#else
	Quat[0] += fUnit.Quat[0];
	Quat[1] += fUnit.Quat[1];
	Quat[2] += fUnit.Quat[2];
	Quat[3] += fUnit.Quat[3];
	Tran[0] += fUnit.Tran[0];
	Tran[1] += fUnit.Tran[1];
	Tran[2] += fUnit.Tran[2];
	Tran[3] += fUnit.Tran[3];
#endif
	return *this;
}

CMatrixUnit	CMatrixUnit::operator * ( const CMatrixUnit& fUnit )
{
	CMatrixUnit Out;
	Out.m_FrameQuat = m_FrameQuat * fUnit.m_FrameQuat;
	Out.m_FrameTrans = fUnit.m_FrameQuat * m_FrameTrans;
	Out.m_FrameTrans += fUnit.m_FrameTrans;
	return Out;
}

CMatrixUnit::operator CMatrix (void)
{
	CMatrix	temp;
	m_FrameQuat.ToRotationMatrix(temp);
	temp.m[3][0] = m_FrameTrans.x;
	temp.m[3][1] = m_FrameTrans.y;
	temp.m[3][2] = m_FrameTrans.z;
	return temp;
}

CMatrixUnit::operator const CMatrix (void)
{
	CMatrix	temp;
	m_FrameQuat.ToRotationMatrix(temp);
	temp.m[3][0] = m_FrameTrans.x;
	temp.m[3][1] = m_FrameTrans.y;
	temp.m[3][2] = m_FrameTrans.z;
	return temp;
}

CMatrix CMatrixUnit::GetMATRIX(void) const
{
	CMatrix	temp;
	m_FrameQuat.ToRotationMatrix(temp);
	temp.m[3][0] = m_FrameTrans.x;
	temp.m[3][1] = m_FrameTrans.y;
	temp.m[3][2] = m_FrameTrans.z;
	return temp;
}

void CMatrixUnit::GetMATRIX(CMatrix& fOut) const
{
	m_FrameQuat.ToRotationMatrix(fOut);
	fOut.m[3][0] = m_FrameTrans.x;
	fOut.m[3][1] = m_FrameTrans.y;
	fOut.m[3][2] = m_FrameTrans.z;
}


#include "stdafx.h"
#include "CMatrix.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	const CMatrix CMatrix::UNIT_MATRIX( 
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );

	//******************************************************************************
	/*! \fn     CMatrix CMatrix::operator * (const CMatrix& mat) const
	*   \brief  矩阵相乘
	*           重载了乘号,完成与传入的矩阵相乘的操作，自身不改变
	*   \param  const CMatrix& mat 为第二(右)操作数
	*   \return CMatrix CMatrix::operator 返回相乘后的结果矩阵
	*******************************************************************************/
	 CMatrix CMatrix::operator * (const CMatrix& mat) const
	{		
#ifdef SIMD
		__declspec(align(16)) CMatrix	Result;
		__asm
		{
			mov		eax,[mat]
			mov		ecx,[this]

			movups	xmm4,[eax]				// Other.M[0][0-3]
			movups	xmm5,[eax+16]			// Other.M[1][0-3]
			movups	xmm6,[eax+32]			// Other.M[2][0-3]
			movups	xmm7,[eax+48]			// Other.M[3][0-3]

			//lea		eax,[Result]

			// Begin first row of result.
			movss	xmm0,[ecx]				// M[0][0] 
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				movss	xmm1,[ecx+4]			// M[0][1]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm5

				movss	xmm2,[ecx+8]			// M[0][2]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm6

				addps	xmm1,xmm0				// First row done with xmm0

				movss	xmm3,[ecx+12]			// M[0][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// Begin second row of result.
				movss	xmm0,[ecx+16]			// M[1][0] 
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				addps	xmm3,xmm2				// First row done with xmm2

				movss	xmm2,[ecx+20]			// M[1][1]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm5

				addps	xmm3,xmm1				// First row done with xmm1

				movss	xmm1,[ecx+24]			// M[1][2]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm6

				movaps	[Result],xmm3				// Store Result.M[0][0-3]
				// Done computing first row.

				addps	xmm2,xmm0				// Second row done with xmm0

				movss	xmm3,[ecx+28]			// M[1][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// Begin third row of result.
				movss	xmm0,[ecx+32]			// M[2][0] 
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				addps	xmm3,xmm1				// Second row done with xmm1

				movss	xmm1,[ecx+36]			// M[2][1]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm5

				addps	xmm3,xmm2				// Second row done with xmm2

				movss	xmm2,[ecx+40]			// M[2][2]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm6

				movaps	[Result+16],xmm3			// Store Result.M[1][0-3]
				// Done computing second row.

				addps	xmm1,xmm0				// Third row done with xmm0

				movss	xmm3,[ecx+44]			// M[2][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// Begin fourth row of result.
				movss	xmm0,[ecx+48]			// M[3][0]
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				addps	xmm3,xmm2				// Third row done with xmm2

				movss	xmm2,[ecx+52]			// M[3][1]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm5

				addps	xmm3,xmm1				// Third row done with xmm1

				movss	xmm1,[ecx+56]			// M[3][2]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm6

				movaps	[Result+32],xmm3			// Store Result.M[2][0-3]
				// Done computing third row.

				addps	xmm2,xmm0

				movss	xmm3,[ecx+60]			// M[3][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// stall

				addps	xmm3,xmm1

				// stall

				addps	xmm3,xmm2

				movaps	[Result+48],xmm3			// Store Result.M[3][0-3]
				// Done computing fourth row.
		}
		return Result;
#else
		CMatrix	Result;

		Result.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
		Result.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
		Result.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
		Result.m[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];

		Result.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
		Result.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
		Result.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
		Result.m[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];

		Result.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
		Result.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
		Result.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
		Result.m[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];

		Result.m[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
		Result.m[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
		Result.m[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
		Result.m[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];
		
		return Result;
#endif
	}

	//******************************************************************************
	/*! \fn     CMatrix& CMatrix::operator *= (const CMatrix& mat)
	*   \brief  矩阵自乘
	*
	*           重载了*=号,完成与传入的矩阵相乘的操作，自身改变
	*   \param  const CMatrix& mat 为第二(右)操作数
	*   \return CMatrix& CMatrix::operator 返回的是相乘后的结果也是自身的引用
	*******************************************************************************/
	CMatrix& CMatrix::operator *= (const CMatrix& mat)
	{
#ifdef SIMD
		__asm
		{
			mov		eax,[mat]
			mov		ecx,[this]

			movups	xmm4,[eax]				// Other.M[0][0-3]
			movups	xmm5,[eax+16]			// Other.M[1][0-3]
			movups	xmm6,[eax+32]			// Other.M[2][0-3]
			movups	xmm7,[eax+48]			// Other.M[3][0-3]

			// Begin first row of result.
			movss	xmm0,[ecx]				// M[0][0] 
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				movss	xmm1,[ecx+4]			// M[0][1]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm5

				movss	xmm2,[ecx+8]			// M[0][2]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm6

				addps	xmm1,xmm0				// First row done with xmm0

				movss	xmm3,[ecx+12]			// M[0][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// Begin second row of result.
				movss	xmm0,[ecx+16]			// M[1][0] 
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				addps	xmm3,xmm2				// First row done with xmm2

				movss	xmm2,[ecx+20]			// M[1][1]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm5

				addps	xmm3,xmm1				// First row done with xmm1

				movss	xmm1,[ecx+24]			// M[1][2]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm6

				movups	[ecx],xmm3				// Store M[0][0-3]
				// Done computing first row.

				addps	xmm2,xmm0				// Second row done with xmm0

				movss	xmm3,[ecx+28]			// M[1][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// Begin third row of result.
				movss	xmm0,[ecx+32]			// M[2][0] 
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				addps	xmm3,xmm1				// Second row done with xmm1

				movss	xmm1,[ecx+36]			// M[2][1]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm5

				addps	xmm3,xmm2				// Second row done with xmm2

				movss	xmm2,[ecx+40]			// M[2][2]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm6

				movups	[ecx+16],xmm3			// Store M[1][0-3]
				// Done computing second row.

				addps	xmm1,xmm0				// Third row done with xmm0

				movss	xmm3,[ecx+44]			// M[2][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// Begin fourth row of result.
				movss	xmm0,[ecx+48]			// M[3][0]
			shufps	xmm0,xmm0,0
				mulps	xmm0,xmm4

				addps	xmm3,xmm2				// Third row done with xmm2

				movss	xmm2,[ecx+52]			// M[3][1]
			shufps	xmm2,xmm2,0
				mulps	xmm2,xmm5

				addps	xmm3,xmm1				// Third row done with xmm1

				movss	xmm1,[ecx+56]			// M[3][2]
			shufps	xmm1,xmm1,0
				mulps	xmm1,xmm6

				movups	[ecx+32],xmm3			// Store M[2][0-3]
				// Done computing third row.

				addps	xmm2,xmm0

				movss	xmm3,[ecx+60]			// M[3][3]
			shufps	xmm3,xmm3,0
				mulps	xmm3,xmm7

				// stall

				addps	xmm3,xmm1

				// stall

				addps	xmm3,xmm2

				movups	[ecx+48],xmm3			// Store M[3][0-3]
				// Done computing fourth row.
		}
#else
		CMatrix Result;
		Result.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
		Result.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
		Result.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
		Result.m[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];

		Result.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
		Result.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
		Result.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
		Result.m[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];

		Result.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
		Result.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
		Result.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
		Result.m[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];

		Result.m[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
		Result.m[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
		Result.m[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
		Result.m[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];
		*this = Result;

#endif
		return *this;
	}
}


#pragma once
#include "CQuaternion.h"
#include "CVector3.h"
#include "CGraphicMallocObject.h"
namespace sqr
{
	class CMatrixUnit : public CGraphicMallocObject
	{
	public:
		CMatrixUnit() 
			: m_FrameTrans()
			, m_FrameQuat()
			, m_FrameFloat(0.0f)
		{};

		CMatrixUnit(float x,float y,float z,float qx,float qy,float qz,float qw)
			: m_FrameTrans(x,y,z),m_FrameQuat(qw,qx,qy,qz),m_FrameFloat(0.0f)
		{};

		CMatrixUnit(const CMatrixUnit& fUnit)
			: m_FrameQuat(fUnit.m_FrameQuat)
			, m_FrameTrans(fUnit.m_FrameTrans)
			, m_FrameFloat(fUnit.m_FrameFloat)
		{};

		CMatrixUnit(const CMatrix& fMatrix)
			: m_FrameFloat(0.0f)
		{
			m_FrameQuat.FromRotationMatrix(fMatrix);
			m_FrameTrans.x = fMatrix.m[3][0] ;
			m_FrameTrans.y = fMatrix.m[3][1] ;
			m_FrameTrans.z = fMatrix.m[3][2] ;
		}

		union
		{
			struct
			{
				float Quat[4];
				float Tran[4];
			};

			struct
			{
				CQuaternion			m_FrameQuat;
				CVector3f			m_FrameTrans;
				float				m_FrameFloat;
			};
		};

		CMatrixUnit& operator = (const CMatrix& fMat);
		CMatrixUnit& operator = ( const CMatrixUnit& fUnit );
		CMatrixUnit& operator *= ( const CMatrixUnit& fUnit );
		CMatrixUnit& operator *= ( const float fR );
		CMatrixUnit	operator * ( const CMatrixUnit& fUnit );
		CMatrixUnit	operator * ( const float fR );
		CMatrixUnit& operator += ( const CMatrixUnit& fUnit );

		operator CMatrix(void);
		operator const CMatrix(void);
		
		CMatrix GetMATRIX(void) const;
		void GetMATRIX(CMatrix& fOut) const;
		
		inline  void	UpCMatrixUnitToDualQuaternion(void)
		{
			CQuaternion tempQuat;
			tempQuat.x = 0.5f * ( Tran[0] * m_FrameQuat.w + Tran[1] * m_FrameQuat.z - Tran[2] * m_FrameQuat.y ) ;
			tempQuat.y = 0.5f * (-Tran[0] * m_FrameQuat.z + Tran[1] * m_FrameQuat.w + Tran[2] * m_FrameQuat.x ) ;
			tempQuat.z = 0.5f * ( Tran[0] * m_FrameQuat.y - Tran[1] * m_FrameQuat.x + Tran[2] * m_FrameQuat.w ) ;
			tempQuat.w = -0.5f *( Tran[0] * m_FrameQuat.x + Tran[1] * m_FrameQuat.y + Tran[2] * m_FrameQuat.z ) ;
			Tran[0] = tempQuat.x;
			Tran[1] = tempQuat.y;
			Tran[2] = tempQuat.z;
			Tran[3] = tempQuat.w;
		}

		static const CMatrixUnit CMatrixUnitZero;
	};

	inline void MatrixUnitMultiply (CMatrixUnit& Out,const CMatrixUnit& In1,const CMatrixUnit& In2 )
	{
		Out.m_FrameQuat = In1.m_FrameQuat * In2.m_FrameQuat;
		Out.m_FrameTrans = In2.m_FrameQuat* In1.m_FrameTrans;
		Out.m_FrameTrans += In2.m_FrameTrans;
	}
}

#pragma once
#include "CElementManager.h"

namespace sqr
{
	class CLogicData : public CElementNode
	{
		friend class CLogicDataCoder;
	public:
		CLogicData(CElementManager* pParent , URegionID id);
		
		const CVector3f&	GetLogicNormal(uint16 x,uint16 y);
		float				GetLogicHight( uint16 x, uint16 y );
		float				GetWaterHeight( uint16 x, uint16 y );		
		int8				GetMaterialIndex(uint16 x, uint16 y);
	protected:
		typedef SVector<CVector3f>	VectorPool;
		typedef SVector<float>		FloatPool;	
		typedef SVector<int8>		Int8Pool;	
		
		VectorPool					m_aryLogicNormal;
		FloatPool					m_aryLogicHight;
		FloatPool					m_aryWaterHight;
		Int8Pool					m_MaterialIndex;
	};
}
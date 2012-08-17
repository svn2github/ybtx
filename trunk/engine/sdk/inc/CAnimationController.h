#pragma once
#include "GraphicClassDefine.h"
#include "CMatrixUnit.h"
namespace sqr
{
	class CAnimationController : public CGraphicMallocObject
	{
	public:
		CAnimationController( CModel* pModel, int32 SkeId );
		CAnimationController(void);
		virtual ~CAnimationController(void);

		void SetCtrlModel( CModel* pModel, int32 SkeId );
		void DoControl( CMatrixUnit& CurMatrix , 
			const CMatrixUnit& ParentMatrix,
			const CSkeletalFrame& SkeFrame,
			CAnimate* pAnimate,
			float fCurFrame ,
			uint32 uCurTime );//临时就先这样
		void Release( void );
	protected:
		virtual void _DoControl( CMatrixUnit& CurMatrix , 
			const CMatrixUnit& ParentMatrix,
			const CSkeletalFrame& SkeFrame,
			CAnimate* pAnimate,
			float fCurFrame, 
			uint32 uCurTime ) = 0;//临时就先这样,再大就传CRenderMatrix了

		CModel* m_pModel;
		int32	m_SkeId;
		uint32	m_uCurTime;
	};
}
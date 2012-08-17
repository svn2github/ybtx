#pragma once
#include "CGeometryInstance.h"
#include "CBaseModel.h"
#include "CSceneMallocObject.h"

namespace sqr
{
	class CModelInst : public CBaseModel
	{
	public:
		CModelInst(ILogicHandler* pHandler = NULL);
		~CModelInst();
	public:
		typedef SList< CGeometryInst > GeometryInstList;
		
	public:		
		void ClearModelRender(void);		
		bool SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop = true, int32 DelayTime = 200 /*∫¡√Î*/, float AniSpeed = -1.0f );
		const AniResult Render( uint32 uCurTime, RenderObjStyle* pRORS = NULL );	//‰÷»æƒ£–Õ
		void  SetFixMatrix(const CMatrix& parent, const CMatrix& fix = CMatrix::UNIT_MATRIX);
		void  UpdateBox(void);
	protected:
		void				_UpdateSkeFrm(void);
		void				_UpdateAniGrp(void);
		void				_UpdateMesh(SKELETAL_MASK& SkeletalMask);
		bool				m_bNeedUpdate;
		GeometryInstList	m_mapInst;
		INST_ANI_PARAMS		m_InstAniParams;
		CMatrix				m_RenderMat;
	};
}
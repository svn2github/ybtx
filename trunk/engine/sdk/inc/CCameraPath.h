#pragma once
#include "CHermiteCurve.h"
#include "CCameraController.h"

namespace sqr
{
	class CCamera;
	class CBufFile;

	class CCameraPath : public CCameraController
	{
	public:
		static const float LOOP_KEY;

	public:
		CCameraPath();
		virtual ~CCameraPath();

	public:		
		void	Build();
		void	Clear();

		void	AddPositionKey( CVector3f key, const float fNewKeyTime );
		void	AddTargetkey( CVector3f key, const float fNewKeyTime );
		void	AddFovKey( CVector3f key, const float fNewKeyTime );

	protected:
		CtrlState		_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );
		CHermiteCurve	m_PositionCurve;
		CHermiteCurve	m_TargetCurve;
		CHermiteCurve	m_FovCurve;
	};
}
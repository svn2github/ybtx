#pragma once
#include "CCameraBase.h"
#include "CGameGacCameraNode.h"

namespace sqr
{
	class CCameraManager;
	class CRTSCameraCtrller;

	class CGameGacCamera : public CCameraBase
	{
		friend class CGameGacCameraNode;

	public:
		enum EType
		{
			eTP_Position,
			eTP_Target,
		};

	private:
		CRTSCameraCtrller  *m_pRTSCameraCtrller;
		float			   m_fCamDist;
		CGameGacCameraNode* EventProc;

	protected:
		CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

	private:
		CGameGacCamera& operator = ( const CGameGacCamera & );
		CGameGacCamera( const CGameGacCamera & );
		void		LoadGameCameraConfig();

	public:
		CGameGacCamera(CCameraManager* camMgr);
		~CGameGacCamera();
		
		void		_DoCameraCtrl( uint32 uCurTime );
		void		MoveLeft();
		void		MoveRight();
		void		MoveFront();
		void		MoveBack();

		void		Move( CVector3f v );

		void		BindGameCamera( const DWORD time );

		void		SetCameraOffset(const float MouseDelta);
		void		SetCameraTarget(const CVector3f& target);

		void		SetCameraYRotate( float dgree );
		float		GetCameraYRotate();

		CGameGacCameraNode* GetEventPorcess() const;

		void		DoCtrlUpdate(uint32 uCurTime);
	};
}
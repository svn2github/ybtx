#pragma once
#include "CCameraBase.h"
#include "CRecordCameraNode.h"

namespace sqr
{
	class CCameraManager;
	class CRecordCameraNode;

	class CRecordCamera : public CCameraBase
	{
		friend class CRecordCameraNode;

	public:
		enum EType
		{
			eTP_Position,
			eTP_Target,
		};
	private:
		CRecordCameraNode* EventProc;
		EType type;

	private:
		CRecordCamera& operator = ( const CRecordCamera & );
		CRecordCamera( const CRecordCamera & );
		
		CtrlState   _DoCameraCtrl( uint32 uCurTime, CCamera* pCamera, CSceneQuery* pQuery );
		void		_RotatePosition( float yaw, float pitch );
		void		_RotateTarget( float yaw, float pitch );

	public:
		CRecordCamera(CCameraManager* camMgr);
		~CRecordCamera();
		
		void		MoveLeft();
		void		MoveRight();
		void		MoveFront();
		void		MoveBack();

		void		Move( CVector3f v );
		void		Rotate( float yaw, float pitch );
		void		Zoom( float fValue );

		void		SwitchToStandard( const CVector3f& vPos );
		void		SetPitchAndYaw( float fPitch, float fYaw );

		void		SetType( EType eType );

		CRecordCameraNode* GetEventPorcess() const;

		void        DoCtrlUpdate(uint32 uCurTime);
	};


}
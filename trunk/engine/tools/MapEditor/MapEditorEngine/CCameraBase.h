#pragma once
#include "CCameraController.h"

namespace sqr
{
	enum ECamType
	{
		eCT_MapEdit = 0,
		eCT_FirstView,
		eCT_GameView,
		eCT_None,
		eCT_Count
	};

	class CCameraManager;
	class CCameraBaseImpl;

	class CCameraBase : public CCameraController
	{
		friend class CCameraBaseImpl;

	protected:
		float			 m_fNearClip;
		float			 m_fFarClip;
		float			 m_fNewFov;
		float			 m_fNewPitch;
		float			 m_fNewYaw;
		float			 m_fMovSpeed;
		bool			 m_bInitBind;
		CVector3f		 m_NewTarget;
		CVector3f		 m_NewPosition;
		CCameraManager*  m_pCamMgr;

		ECamType         m_camType;

	private:
		CCameraBase( const CCameraBase & );
		CCameraBase& operator = ( const CCameraBase & );

	protected:
		CCameraBase(CCameraManager* camMgr);
		
		void _InitBind();

	public:
		virtual		~CCameraBase();

		void        SetCamType(const ECamType& type);
		ECamType    GetCamType() const;

		void		SetNewPosition( const CVector3f& vPos );
		void		SetNewTarget( const CVector3f& vTarget );
		void		SetPosition( const CVector3f& vPos );
		void		SetTarget( const CVector3f& vTarget );
		CVector3f	GetNewPosition()	const;
		CVector3f	GetNewTarget()		const;
		CVector3f	GetPosition()		const;
		CVector3f	GetTarget()			const;
		CVector3f   GetDirection()		const;

		float		GetPitch()			const;
		float		GetYaw()			const;
		float		GetDistance()		const;
		void		SetFovY( float fFOVY );
		float		GetFovY()			const;
		void		IncreaseTheFOV();
		void		DecreaseTheFOV();

		void		SetMovSpeed(float fSpeed);
		float		GetMovSpeed()		const;

		bool		IsValid() const;

		virtual void        DoCtrlUpdate(uint32 uCurTime){};
	};
}
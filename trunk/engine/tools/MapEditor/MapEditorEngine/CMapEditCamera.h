#pragma once
#include "Scene.h"
#include "CCamera.h"
#include "CCameraBase.inl"
#include "CMapEditCameraNode.h"
#include "MEGraphic.h"

namespace sqr
{
	class CMapEditCameraImpl;
	class CCameraManager;
	
	class CMapEditCamera : public CCameraBase
	{
		friend class CMapEditCameraNode;

	protected:
		CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

		CMapEditCameraNode* EventProc;

		bool m_bYBTXCamera;
		bool m_bOrthogonal;
		float m_fPatchWidth;
		float m_fPatchHeight;

	public:
		CMapEditCamera(CCameraManager* camMgr);
		virtual		~CMapEditCamera();

		void		MoveLeft();
		void		MoveRight();
		void		MoveFront();
		void		MoveBack();

		void		MoveRTS(CVector3f v);
		void		MoveToRTSByView(CVector3f v);
		void		RotatePosition(float yaw, float pitch);
		void		Zoom(float fValue);
		
		void		AdjustViewY(bool bUp);

		void		SetPitchAndYaw(float fPitch, float fYaw, bool bLogic = false);
		void		SetPitch(float pitch);
		void		SetYaw(float yaw);

		CVector3f	GetDirection();
		

		void		SetIsOrthogonal(bool b);
		void		SetIsRotatable(bool b);
		bool		GetIsOrthogonal();
		bool		GetIsRotatable();

		void		SwitchToStandard(const CVector3f& vTarget);
		void		SwitchToLogicStandard(const CVector3f& vTarget);
		void		SwitchToMiniMapStandard(const CVector3f& vTarget, float fPitch);
		void		SwitchToTopViewMapStandard(const CVector3f& vTarget, float fPitch);
		void		SwitchToLeftMapStandard(const CVector3f& vTarget);
		void		SwitchToFrontMapStandard(const CVector3f& vTarget);
		void		SwitchToSkyStandard();

		void		IncreaseTheView();
		void		DecreaseTheView();
		void		SetFarClip(const float fFarClip);
		float       GetFarClip() const;

		void		AdjustBothY(bool bUp);

		void		SetPatchWidth(float f);
		void		SetPatchHeight(float f);

		float		GetPatchWidth();
		float		GetPatchHeight();

		void		SetIsGameView(bool b);
		void		SetGameDistanceRange(float max, float min);

		CMapEditCameraNode* GetEventPorcess() const;

		void        DoCtrlUpdate(uint32 uCurTime);
	};
}

#pragma once
#include "BaseBrush.h"

namespace sqr
{
	class CSceneBoxDebugNode;
}

namespace sqr_tools
{
	class CEditModel;
	class CTerrainModelBrush : public CBaseBrush
	{
	public:

		CTerrainModelBrush(void);
		~CTerrainModelBrush(void);

		virtual void MouseMoveEvent(MSG_MOUSE_INF mouse);
		virtual void MoveLButtonDownEvent(MSG_MOUSE_INF mouse);
		virtual void LButtonUpEvent(MSG_MOUSE_INF mouse);
		virtual void CtrlKeyDownEvent(MSG_KEY_INF key);
		virtual void LShiftKeyDownEvent(MSG_KEY_INF key);
		virtual bool KeyDownEvent(MSG_KEY_INF key);
		virtual void KeyUpEvent(MSG_KEY_INF key);

		void	CreateActiveModel(const vector<string>& vecEditModelFileNames, const string& strAniName, const string& strEffectFileName, const string& strEffectName, bool bVisibleByConfigure);
		void	SetActiveModel(CEditModel* pActiveModel);
		void	SetLockModel(CEditModel* pLockModel);
		void	ShowLockModeWireBox();
		void	PlaceActiveModel();
		void	ClearActiveObject();
		void	DelectLockObject();
		void	ZoomInModelSize();
		void	ZoomOutModelSize();
		void	NormalModelSize();
		void	RotateModelAxisX(const bool bPositive);
		void	RotateModelAxisY(const bool bPositive);
		void	RotateModelAxisZ(const bool bPositive);
		void	RevertModelRotation();
		void	ApeakRotateModel();
		void	MoveModelAxisX(const bool bPositive);
		void	MoveModelAxisY(const bool bPositive, const float fStep);
		void	MoveModelAxisZ(const bool bPositive);
		void	OnNextRenderStyle();
		void	OnSwitchShadow();
		void	OnSwitchReceiveShadow();
		void	OnPlayAniOnOff();
		void	OnSwitchUseCameraCollision();
		void	SelectModelsByBrush();
		void	CopySelectModels();
		void	PasteSelectModels();

		void	SetActiveModelState(const EEdit_State state)
		{
			m_eActiveModelState = state;
		}

	private:

		typedef vector<CEditModel*> ModelPool;
		
		ModelPool			m_vecActiveModels;
		ModelPool			m_vecLockModels;
		EEdit_State			m_eActiveModelState;
		CSceneBoxDebugNode* m_pSceneBoxDebuger;
	};
}

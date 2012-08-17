#pragma once
#include "CCoreViewImp.h"

class CLightViewHelper;

namespace sqr
{	
	class CRenderList;
	class CSceneGridDebugNode;
	class CLight;
}

namespace sqr_tools
{		
	class CEditContext;
	class CRTSCamDelegate;
	class CTPSCamDelegate;

	enum LIGHT_CTRL_STATE
	{
		STATE_MOVE,
		STATE_ROT,
		STATE_DEFAULT
	};

class CLightViewImp :public CCoreViewImp
{
	REG_UI_IMP(CLightViewImp);
public:
	CLightViewImp(void* _handle);
	~CLightViewImp(void);

	virtual void SetContext(CEditContext* pContext);
	virtual void InitLightScene();

#pragma region IMP_DETIAL
	//init
	virtual void initSphere();
	virtual void initCone();
	virtual void initCtrllerLight();
	//update
	virtual void UpdateCamPos();
	virtual void UpdateCtrllerLight();
	virtual void UpdateCone();
	//reset
	virtual void Ensure();
	virtual void Reset();
	//ctrl
	virtual void SetVisible(bool vis);	
	virtual void calculateLightPos(int mouse_x,int mouse_y);
#pragma endregion

	//Msg
	virtual bool OnCtrlMouseMove(int x,int y);
	virtual bool OnCtrlLBtDown(int x,int y);
	virtual bool OnCtrlLBtUp(int x,int y);
	
	virtual bool OnCtrlRBtDown(int x,int y);
	virtual bool OnCtrlRBtUp(int x,int y);
	
	virtual bool OnCtrlMBtDown(int x,int y);
	virtual bool OnCtrlMBtUp(int x,int y);
	
	virtual bool OnCtrlMouseWheel(int delta);

	virtual void SetAmbientColor(int color);
	virtual void SetSunColor(int color);
	virtual void SetPlayerAmbientColor(int color);
	virtual void SetPlayerSunColor(int color);

protected:
	CLightViewHelper* m_Helper;

	//这个是光向场景的Context
	CEditContext*			m_LightContext;
	CSceneGridDebugNode*	m_BackGrid;
	CTPSCamDelegate*		m_LigthCamTPSDele;
	
	LIGHT_CTRL_STATE	m_State;
	int					m_LastMousePosX;
	int					m_LastMousePosY;
};

}
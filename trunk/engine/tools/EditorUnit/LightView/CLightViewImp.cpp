#include "StdAfx.h"
#include "LightView\CLightViewImp.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CEditContext.h"
#include "CFilterNode.h"
#include "CEditorCore.h"
#include "CRenderCommand.h"
#include "CColor.h"
#include "CColor4.inl"
#include "CLight.h"
#include "CTPSCameraCtrller.h"
#include "CameraCtrl\CTPSCamDelegate.h"
#include "CCamera.h"
#include "CEventOwner.h"

#include "CSceneGridDeubg.h"
#include "CameraCtrl\CRTSCamDelegate.h"

const float DEFAULT_FOV = CMath::ToRadian(30.0f);
const float SPHERE_SIZE = 150.0f;
const float CONE_SIZE	= 64.0f;

class CLightViewHelper
{
public:
	CLightViewHelper()
	{
		light_ctrl=0;
		ctrl_state=CS_OUTCTRL;
	}

	~CLightViewHelper()
	{
	}

	enum CtrlState
	{
		CS_OUTCTRL,
		CS_MOVE,
		CS_ZOOM,
		CS_ROTATE,
	};

	NodePool			sphere_render_list;
	NodePool			cone_render_list;
	CtrlState			ctrl_state;
	CSphere				ref_sphere;			// 参考球，用于鼠标点击的碰撞检测	
	
	CLight*				light_ctrl;
	CLight				light_out;
};

CColor ColorTrans(CColor4 color)
{
	CColor tmpColor;
	tmpColor.A=unsigned char(255*color.a);
	tmpColor.R=unsigned char(255*color.r);
	tmpColor.G=unsigned char(255*color.g);
	tmpColor.B=unsigned char(255*color.b);
	return tmpColor;
}

void NodeRender(CRenderCommand& Com)
{
	int i = 0;
}

DEF_UI_IMP(CLightViewImp)

CLightViewImp::CLightViewImp(void* _handle)
:CCoreViewImp(_handle)
{
	m_LightContext=new CEditContext();
	m_LigthCamTPSDele=new CTPSCamDelegate();
	m_LightContext->RegisterDelegate(m_LigthCamTPSDele);
	m_Helper=new CLightViewHelper();
	m_LastMousePosX=0;
	m_LastMousePosY=0;
	m_State=STATE_DEFAULT;
}

CLightViewImp::~CLightViewImp(void)
{
	delete m_Helper;
	if(m_LightContext)
	{
		m_LightContext->DelelteDebugNode(m_BackGrid);
		m_BackGrid=NULL;
		m_LightContext->UnRegisterDelegate(m_LigthCamTPSDele);
		delete m_LigthCamTPSDele;
	}
}

void CLightViewImp::SetContext(CEditContext* pContext)
{
	//主场景的Contexnt
	m_pOwnContext=pContext;
	if(m_LightContext)
	{
		m_LightContext->SetRenderTarget(m_pWindowTarget);
		SetOwner(m_LightContext);
	}
	InitLightScene();
}

void CLightViewImp::InitLightScene()
{
	//注册表格并显示
	m_BackGrid = new CSceneGridDebugNode(ROS_LIGHTMAT1,m_LightContext);
	m_LightContext->RegistDebugNode(m_BackGrid);
	//相机初始化
	CCamera* pCamera = m_pRenderGroup->GetCamera();
	pCamera->setNearDist(10.0f);
	pCamera->setFarDist(10000.0f);
	pCamera->setFOVy(DEFAULT_FOV);
	m_LigthCamTPSDele->SetMaxFov( DEFAULT_FOV );
	m_LigthCamTPSDele->SetMinFov( DEFAULT_FOV );
	m_LigthCamTPSDele->SetTargetDist(640.0f);
	m_LigthCamTPSDele->SetResetCtrlTime(0);
	m_LigthCamTPSDele->BeginCtrl((uint32)GetProcessTime());
	//显示光向球和浮标
	initSphere();
	initCone();
	initCtrllerLight();
}

void CLightViewImp::initSphere()
{
	FilterNode sphere_node; 
	sphere_node = CEditorCore::TP_RO_SPHERE;
	sphere_node.m_eType							= ROS_LIGHTMAT1;
	sphere_node.m_RP.m_RS.m_LightEnable			= TRUE;
	sphere_node.m_RP.m_RS.m_SpecularEnable		= TRUE;
	sphere_node.m_RP.m_RS.m_Material.Diffuse	= CColor4::White;
	sphere_node.m_RP.m_RS.m_Material.Specular	= CColor4::White;
	sphere_node.m_RP.m_RS.m_Material.Ambient	= CColor4::Black;
	sphere_node.m_RP.m_RS.m_Material.Power		= 180;
	sphere_node.m_RP.m_RS.m_AlphaBlendEnable	= FALSE;
	sphere_node.param_render					= NodeRender;
	sphere_node.m_RP.m_Mat.SetScale(SPHERE_SIZE, SPHERE_SIZE, SPHERE_SIZE);
	m_pRenderGroup->Begin(&m_Helper->sphere_render_list);
	m_pRenderGroup->InsertRender(&sphere_node);
	m_pRenderGroup->End();
	m_Helper->ref_sphere.setCenter(CVector3f(0,0,0));
	m_Helper->ref_sphere.setRadius(SPHERE_SIZE);
}

void CLightViewImp::initCone()
{
	FilterNode cone_node;
	cone_node = CEditorCore::TP_RO_CONE;
	cone_node.m_eType						= ROS_ALPHA_LIGHT1;
	cone_node.m_RP.m_RS.m_LightEnable		= TRUE;
	cone_node.m_RP.m_RS.m_SpecularEnable	= TRUE;
	cone_node.m_RP.m_RS.m_AlphaBlendEnable	= TRUE;
	cone_node.m_RP.m_RS.m_SrcBlend			= BLEND_INVDESTCOLOR;
	cone_node.m_RP.m_RS.m_DestBlend			= BLEND_ZERO;
	cone_node.m_RP.m_RS.m_Material.Diffuse	= CColor4::White;
	cone_node.m_RP.m_RS.m_Material.Ambient	= CColor4::Red;
	cone_node.m_RP.m_RS.m_Material.Specular	= CColor4::White;
	cone_node.m_RP.m_RS.m_Material.Power	= 80;
	cone_node.m_RP.m_Mat.Identity();
	m_pRenderGroup->Begin(&m_Helper->cone_render_list);
	m_pRenderGroup->InsertRender(&cone_node);
	m_pRenderGroup->End();
}

void CLightViewImp::initCtrllerLight()
{	
	if(m_Helper)
	{
		m_Helper->light_out.GetLightStruct()->Ambient = m_pOwnContext->GetWindowTarget()->GetGroup()->GetLightSystem()->GetGlobalLight()->GetLightStruct()->Ambient;
		m_Helper->light_out.GetLightStruct()->Diffuse = m_pOwnContext->GetWindowTarget()->GetGroup()->GetLightSystem()->GetGlobalLight()->GetLightStruct()->Diffuse;
		m_Helper->light_out.setDIRECTION( m_pOwnContext->GetWindowTarget()->GetGroup()->GetLightSystem()->GetGlobalLight()->getDIRECTION());
		m_Helper->light_out.SetPosition( m_pOwnContext->GetWindowTarget()->GetGroup()->GetLightSystem()->GetGlobalLight()->GetPosition() );
		
		m_Helper->light_ctrl= m_pRenderGroup->GetLightSystem()->GetGlobalLight();
		Reset();
	}
}

void CLightViewImp::Ensure()
{
	if(m_Helper &&  m_Helper->light_ctrl)
	{
		m_Helper->light_out.GetLightStruct()->Ambient=m_Helper->light_ctrl->GetLightStruct()->Ambient;
		m_Helper->light_out.GetLightStruct()->Diffuse=m_Helper->light_ctrl->GetLightStruct()->Diffuse;
		m_Helper->light_out.setDIRECTION(m_Helper->light_ctrl->getDIRECTION());
		m_Helper->light_out.SetPosition(m_Helper->light_ctrl->GetPosition());
	}
}

void CLightViewImp::Reset()
{
	if(m_Helper &&  m_Helper->light_ctrl)
	{
		//重置颜色和光向
		SetAmbientColor(ColorTrans(m_Helper->light_out.GetLightStruct()->Ambient).dwColor);
		SetSunColor(ColorTrans(m_Helper->light_out.GetLightStruct()->Diffuse).dwColor);
		m_Helper->light_ctrl->setDIRECTION(m_Helper->light_out.getDIRECTION());
		m_Helper->light_ctrl->SetPosition(m_Helper->light_out.GetPosition());
		//重置骨骼位置
		UpdateCone();
	}
}

void CLightViewImp::UpdateCamPos()
{
	if(!m_pOwnContext)
		return ;
	CCamera*	main_camera = m_pOwnContext->GetWindowTarget()->GetCamera();
	CVector3f	dir			= main_camera->getDIRECTION();
	dir = dir - dir.Dot(CVector3f::UNIT_Y)*CVector3f::UNIT_Y;
	dir.Normalize();

	float y_rad=CMath::ACos(dir.Dot(CVector3f::UNIT_X));
	if ( dir.z < 0 )
		y_rad = CMath::two_pi - y_rad;
	y_rad = -y_rad + CMath::half_pi;

	m_LigthCamTPSDele->SetYRotateDegree(y_rad);
	m_LigthCamTPSDele->DoCtrl((uint32)GetProcessTime());
}

void CLightViewImp::UpdateCtrllerLight()
{
	if(m_Helper &&  m_Helper->light_ctrl)
	{
		m_LightContext->SetAmbientColor( ColorTrans(m_Helper->light_ctrl->GetLightStruct()->Ambient) );
		m_LightContext->SetSunLightColor( ColorTrans(m_Helper->light_ctrl->GetLightStruct()->Diffuse) );
		m_LightContext->SetSunLightDir( m_Helper->light_ctrl->getDIRECTION());
		UpdateCone();
	}
}

void CLightViewImp::UpdateCone()
{
	FilterNode& cone = m_Helper->cone_render_list.back();
	CQuaternion quat;
	CMatrix		mov_mat;
	quat.FromTwoVector(CVector3f::UNIT_Z, m_Helper->light_ctrl->getDIRECTION());
	quat.ToRotationMatrix(mov_mat);
	mov_mat.getColumn(3) = m_Helper->light_ctrl->GetPosition();
	cone.m_RP.m_Mat.SetScale(CVector3f(CONE_SIZE, CONE_SIZE, CONE_SIZE));
	cone.m_RP.m_Mat *= mov_mat;
}

void CLightViewImp::SetVisible(bool vis)
{
}

void CLightViewImp::calculateLightPos(int mouse_x,int mouse_y)
{
	if(m_Helper && m_Helper->light_ctrl)
	{
		CCamera*	camera = m_pRenderGroup->GetCamera();
		CMatrix		view_mat_invert	= camera->getViewMatrix();
		view_mat_invert.Invert();
		CIRect		rect;
		CVector3f	ray_dir;
		CVector3f	ray_pos;
		CVector3f	hit_pos;
		bool		is_intersect;

		rect=GetViewRect();
		ray_dir.x =  ( 2.0f * mouse_x / rect.Width()  - 1.0f ) / camera->getProjectionMatrix()._11;
		ray_dir.y = -( 2.0f * mouse_y / rect.Height() - 1.0f ) / camera->getProjectionMatrix()._22;
		ray_dir.z =  1.0f;
		ray_dir.Rotate(view_mat_invert);
		ray_dir.Normalize();
		ray_pos	= view_mat_invert.getColumn(3);	
		is_intersect = m_Helper->ref_sphere.intersects(hit_pos, ray_pos, ray_dir);
		if ( !is_intersect )
			return;
		CVector3f light_dir = m_Helper->ref_sphere.getCenter() - hit_pos;
		light_dir.Normalize();
		m_Helper->light_ctrl->SetPosition(hit_pos);
		m_Helper->light_ctrl->setDIRECTION(light_dir);	
		//数据刷到主场景中
		if(m_pOwnContext)
			m_pOwnContext->SetSunLightDir( light_dir );
	}
}

void CLightViewImp::SetAmbientColor(int color)
{
	CColor tmpColor;
	tmpColor.dwColor=color;
	if(m_pOwnContext)
		m_pOwnContext->SetAmbientColor(tmpColor);
	if(m_Helper && m_Helper->light_ctrl)
	{
		m_Helper->light_ctrl->GetLightStruct()->Ambient=CColor4(tmpColor);
		UpdateCtrllerLight();
	}

}

void CLightViewImp::SetSunColor(int color)
{
	CColor tmpColor;
	tmpColor.dwColor=color;
	if(m_pOwnContext)
		m_pOwnContext->SetSunLightColor(tmpColor);
	if(m_Helper && m_Helper->light_ctrl)
	{
		m_Helper->light_ctrl->GetLightStruct()->Diffuse=CColor4(tmpColor);
		UpdateCtrllerLight();
	}
}

void CLightViewImp::SetPlayerAmbientColor(int color)
{
	CColor tmpColor;
	tmpColor.dwColor=color;
	if(m_pOwnContext)
		m_pOwnContext->SetPlayerAmbientColor(tmpColor);
	if(m_LightContext)
		m_LightContext->SetPlayerAmbientColor(tmpColor);
}

void CLightViewImp::SetPlayerSunColor(int color)
{
	CColor tmpColor;
	tmpColor.dwColor=color;
	if(m_pOwnContext)
		m_pOwnContext->SetPlayerSunColor(tmpColor);
	if(m_LightContext)
		m_LightContext->SetPlayerSunColor(tmpColor);
}

bool CLightViewImp::OnCtrlMouseMove(int x,int y)
{
	if(m_State==STATE_MOVE)
	{
		calculateLightPos(x,y);
		UpdateCone();
	}
	if(m_State == STATE_ROT)
	{
		m_LigthCamTPSDele->SetXRotateDegree( m_LigthCamTPSDele->GetXRotateDegree() + float(y - m_LastMousePosY)/100.0f );
		m_LigthCamTPSDele->DoCtrl((uint32)GetProcessTime());
		//updateLightHandler();
		m_LastMousePosX=x;
		m_LastMousePosY=y;
	}

	return true;
}
bool CLightViewImp::OnCtrlLBtDown(int x,int y)
{
	m_State=STATE_MOVE;
	calculateLightPos(x,y);
	UpdateCone();
	return true;
}
bool CLightViewImp::OnCtrlLBtUp(int x,int y)
{
	m_State=STATE_DEFAULT;
	return true;
}

bool CLightViewImp::OnCtrlRBtDown(int x,int y)
{
	m_State=STATE_ROT;
	m_LastMousePosX=x;
	m_LastMousePosY=y;
	return true;
}
bool CLightViewImp::OnCtrlRBtUp(int x,int y)
{
	m_State=STATE_DEFAULT;
	return true;
}

bool CLightViewImp::OnCtrlMBtDown(int x,int y)
{
	//UpdateCamPos();
	Reset();
	return true;
}
bool CLightViewImp::OnCtrlMBtUp(int x,int y)
{
	return true;
}
bool CLightViewImp::OnCtrlMouseWheel(int delta)
{
	return true;
}
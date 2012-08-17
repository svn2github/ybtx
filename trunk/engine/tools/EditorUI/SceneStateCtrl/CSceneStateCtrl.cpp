#include "StdAfx.h"
#include "CSceneStateCtrl.h"
#include "SceneStateImp/CSceneStateImp.h"

System::Void CSceneStateCtrl::StateSwitch_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{	
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		info._iinfo=0;
		if(this->pStateSwitch->Checked)
			type.custom_type=MGT_SCENESTATE_ENABLE;
		else
			type.custom_type=MGT_SCENESTATE_DISABLE;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CSceneStateCtrl::CountSwitch_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		info._iinfo=0;
		if(this->pCountSwitch->Checked)
			type.custom_type=MGT_SCENECOUNT_ENABLE;
		else
			type.custom_type=MGT_SCENECOUNT_DISABLE;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CSceneStateCtrl::radioFull_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		info._iinfo=0;
		if(this->radioFull->Checked)
			type.custom_type=MGT_SCENESTATE_FULL;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CSceneStateCtrl::radioAlpha_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		info._iinfo=0;
		if(this->radioAlpha->Checked)
			type.custom_type=MGT_SCENESTATE_ALPHA;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}


char* CSceneStateCtrl::GetImpTypeName( void )
{
	return NULL;
}
#include "StdAfx.h"
#include "CVideoSettingCtrl.h"
#include "VideoSettingImp/CVideoSettingImp.h"

System::Void CVideoSettingCtrl::Update()
{
	if(!m_pCtrl)
		return ;
	VideoSettingState state;
	state.AliaxLevel=((CVideoSettingImp*)m_pCtrl)->GetAliaxLevel();
	state.EffectLevel=((CVideoSettingImp*)m_pCtrl)->GetEffectLevel();
	state.ShadowLevel=((CVideoSettingImp*)m_pCtrl)->GetShadowLevel();
	state.TerrainLevel=((CVideoSettingImp*)m_pCtrl)->GetTerrainLevel();
	state.WaterLevel=((CVideoSettingImp*)m_pCtrl)->GetWaterLevel();
	state.TexLevel=((CVideoSettingImp*)m_pCtrl)->GetTexLevel();
	state.IsBloom=((CVideoSettingImp*)m_pCtrl)->GetIsBloom();
	state.IsRefract=((CVideoSettingImp*)m_pCtrl)->GetIsRefract();
	state.IsSyn=((CVideoSettingImp*)m_pCtrl)->GetIsSyn();

	//反走样
	if(state.AliaxLevel == 0)
		this->Alias_Shut->Checked=true;
	else if(state.AliaxLevel==1)
		this->Alias_Low->Checked=true;
	else
		this->Alias_High->Checked=true;

	//特效等级
	if(state.EffectLevel == 0)
		this->Effect_Low->Checked=true;
	else if(state.EffectLevel == 1)
		this->Effect_Mid->Checked=true;
	else
		this->Effect_High->Checked=true;
	
	//阴影等级
	if(state.ShadowLevel == 0)
		this->Shadow_Shut->Checked=true;
	else if(state.ShadowLevel == 1)
		this->Shadow_Low->Checked=true;
	else
		this->Shadow_High->Checked=true;

	//地表质量
	if(state.TerrainLevel == 0)
		this->Terrain_Low->Checked=true;
	else
		this->Terrain_High->Checked=true;

	//水质量
	if(state.WaterLevel == 0)
		this->Water_Shut->Checked=true;
	else
		this->Water_Default->Checked=true;

	//纹理质量
	if(state.TexLevel == 0)
		this->Tex_Low->Checked=true;
	else if(state.TexLevel == 1)
		this->Tex_Mid->Checked=true;
	else
		this->Tex_High->Checked=true;

	//全屏泛光
	this->BloomCheck->Checked=state.IsBloom;

	//反射
	this->RefractCheck->Checked=state.IsRefract;

	//垂直同步
	this->SynCheck->Checked=(state.IsSyn!=1);
}

System::Void CVideoSettingCtrl::SendUpdate()
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		type.custom_type=MGT_VIDEOSET_UPDATE;
		info._iinfo=0;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CVideoSettingCtrl::SendReset()
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		type.custom_type=MGT_VIDEOSET_RESET;
		info._iinfo=0;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CVideoSettingCtrl::SendEnsure()
{
	if(m_pCtrl)
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source=MGS_CUSTOM;
		type.custom_type=MGT_VIDEOSET_ENSURE;
		info._iinfo=0;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void CVideoSettingCtrl::HideButton()
{
	this->bt_app->Visible=false;
	this->bt_reset->Visible=false;
}

System::Void CVideoSettingCtrl::bt_app_Click(System::Object^  sender, System::EventArgs^  e) 
{
	SendUpdate();
}

System::Void CVideoSettingCtrl::bt_reset_Click(System::Object^  sender, System::EventArgs^  e) 
{
	SendReset();
}

System::Void CVideoSettingCtrl::Rate16_9_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Rate16_9->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::Rate16_10_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Rate16_10->Checked)
	{
		int a=0;	
	}
}
System::Void CVideoSettingCtrl::Rate5_4_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Rate5_4->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::Rate4_3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Rate4_3->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::Terrain_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Terrain_Low->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetTerrainLevel(0);
	}
}
System::Void CVideoSettingCtrl::Terrain_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Terrain_High->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetTerrainLevel(1);
	};
}
System::Void CVideoSettingCtrl::FPS_15_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->FPS_15->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::FPS_30_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->FPS_30->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::FPS_45_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->FPS_45->Checked)
		int a=0;
}
System::Void CVideoSettingCtrl::FPS_60_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->FPS_60->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::Water_Shut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Water_Shut->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetWaterLevel(0);
	};
}
System::Void CVideoSettingCtrl::Water_Default_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Water_Default->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetWaterLevel(1);
	};
}
System::Void CVideoSettingCtrl::Effect_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Effect_Low->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetEffectLevel(0);
	}
}
System::Void CVideoSettingCtrl::Effect_Mid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Effect_Mid->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetEffectLevel(1);
	}
}
System::Void CVideoSettingCtrl::Effect_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Effect_High->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetEffectLevel(2);
	}
}
System::Void CVideoSettingCtrl::Tex_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Tex_Low->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetTexLevel(0);
	}
}
System::Void CVideoSettingCtrl::Tex_Mid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Tex_Mid->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetTexLevel(1);
	}
}
System::Void CVideoSettingCtrl::Tex_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Tex_High->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetTexLevel(2);
	}
}
System::Void CVideoSettingCtrl::Shadow_Shut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Shadow_Shut->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetShadowLevel(0);
	}
}
System::Void CVideoSettingCtrl::Shadow_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Shadow_Low->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetShadowLevel(1);
	}
}
System::Void CVideoSettingCtrl::Shadow_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Shadow_High->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetShadowLevel(2);
	}
}
System::Void CVideoSettingCtrl::Alias_Shut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Alias_Shut->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetAliaxLevel(0);
	}
}
System::Void CVideoSettingCtrl::Alias_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Alias_Low->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetAliaxLevel(1);
	}
}
System::Void CVideoSettingCtrl::Alias_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->Alias_High->Checked)
	{
		((CVideoSettingImp*)m_pCtrl)->SetAliaxLevel(2);
	}
}
System::Void CVideoSettingCtrl::WinmodeCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->WinmodeCheck->Checked)
	{
		int a=0;
	}
}
System::Void CVideoSettingCtrl::SynCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(m_pCtrl && this->SynCheck->Checked)
		((CVideoSettingImp*)m_pCtrl)->SetIsSyn(1);
	else
		((CVideoSettingImp*)m_pCtrl)->SetIsSyn(0);	
}
System::Void CVideoSettingCtrl::BloomCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(!m_pCtrl)
		return ;
	if(this->BloomCheck->Checked)
		((CVideoSettingImp*)m_pCtrl)->SetIsBloom(true);
	else
		((CVideoSettingImp*)m_pCtrl)->SetIsBloom(false);	
}
System::Void CVideoSettingCtrl::RefractCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(!m_pCtrl)
		return ;
	if(this->RefractCheck->Checked)
		((CVideoSettingImp*)m_pCtrl)->SetIsRefract(true);
	else
		((CVideoSettingImp*)m_pCtrl)->SetIsRefract(false);
}
System::Void CVideoSettingCtrl::CamCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(!m_pCtrl)
		return ;
	if(this->CamCheck->Checked)
	{
		int a=0;
	}
}

char* CVideoSettingCtrl::GetImpTypeName( void )
{
	return NULL;
}
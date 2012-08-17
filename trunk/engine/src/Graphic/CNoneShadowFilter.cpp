#include "stdafx.h"
#include "CNoneShadowFilter.h"

namespace sqr
{
	CNoneShadowFilter::CNoneShadowFilter(CRenderGroup* pRenderGroup)
		:CShadowFilter(pRenderGroup) 
	{ 
		SetRefInfo("ø’“ı”∞"); 
	};

	void CNoneShadowFilter::ResetFilterQueue(RenderQueueFlag& fQueueFlag)
	{
		fQueueFlag.Delete(ID_SCENE_SHADOWRECEIVET_FILTER);
		fQueueFlag.Delete(ID_NPC_SHADOWRECEIVET_FILTER);
		fQueueFlag.Delete(ID_GEOMETRY_INST_SR_FILTER);
		fQueueFlag.Delete(ID_SHADOW_FILTER);
		fQueueFlag.InsertBefore(ID_TERRAIN_FILTER,ID_SCENE_SHADOWRECEIVET_FILTER);
		fQueueFlag.InsertBefore(ID_SCENE_SHADOWRECEIVET_FILTER,ID_NPC_SHADOWRECEIVET_FILTER);
		fQueueFlag.InsertBefore(ID_NPC_SHADOWRECEIVET_FILTER,ID_GEOMETRY_INST_SR_FILTER);		
	}

	void CNoneShadowFilter::SetShadowParams(STATIC_RS& RS)
	{
		DWORD VS = RS.m_ShaderType.GetVSShaderID();
		DWORD PS = RS.m_ShaderType.GetPSShaderID();
		DWORD AVS =  RS.m_bPreLight?SSI_PRE_RENDER_LIGHT:0;
		VS = VS & ~MakeSysVS_ID(SSI_SHADOW_MAP|AVS);
		PS = PS & ~MakeSysPS_ID(SSI_SHADOW_MAP);
		RS.m_ShaderType.Clear();
		if(VS!=0)RS.m_ShaderType |= MakeSysVS_ID(VS|AVS);
		if(PS!=0)RS.m_ShaderType |= MakeSysPS_ID(PS);
	}
}
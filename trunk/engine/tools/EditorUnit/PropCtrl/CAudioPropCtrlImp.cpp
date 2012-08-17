#include "StdAfx.h"
#include "PropCtrl\CAudioPropCtrlImp.h"
#include "CEditContext.h"
#include "CRenderNode.h"

#define AudioObj ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CAudioPropCtrlImp::CAudioPropCtrlImp(void)
{
	m_PropType=PROP_AUDIO;
}

CAudioPropCtrlImp::~CAudioPropCtrlImp(void)
{
}

//获取音效属性
int CAudioPropCtrlImp::GetSelAudioID()
{
	return 0;
}

float CAudioPropCtrlImp::GetSelAudioPosX()
{
	return 0.0f;
}

float CAudioPropCtrlImp::GetSelAudioPosY()
{
	return 0.0f;
}

float CAudioPropCtrlImp::GetSelAudioPosZ()
{
	return 0.0f;
}

std::string CAudioPropCtrlImp::GetSelAudioName(){ return 0;  }

//获取音效属性
void CAudioPropCtrlImp::SetSelAudioID(int audio_id){   }
void CAudioPropCtrlImp::SetSelAudioPos(float px,float py,float pz){   }
void CAudioPropCtrlImp::SetSelAudioName(std::string name){   }

#pragma once

#include "PropCtrl\CBasePropCtrlImp.h"

namespace sqr_tools
{

class CAudioPropCtrlImp :public CBasePropCtrlImp
{
public:
	CAudioPropCtrlImp(void);
	~CAudioPropCtrlImp(void);

public:
	virtual int GetSelAudioID();
	virtual float GetSelAudioPosX();
	virtual float GetSelAudioPosY();
	virtual float GetSelAudioPosZ();
	virtual std::string GetSelAudioName();
	virtual void SetSelAudioID(int audio_id);
	virtual void SetSelAudioPos(float px,float py,float pz);
	virtual void SetSelAudioName(std::string name);
};

//!namespace
}

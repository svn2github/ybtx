#pragma once

namespace sqr
{
	template<class T> struct TVector3;
	typedef TVector3<float> CVector3f;
}

class LightSettingHandler
{
public:
	virtual void			SetSceneDirColor(uint32 color);
	virtual void			SetSceneAmbColor(uint32 color);
	virtual void			SetNpcDirColor(uint32 color);
	virtual void			SetNpcAmbColor(uint32 color);
	virtual void			SetDir(const CVector3f& dir);
};
#pragma once
#include "CVector3.h"

namespace bly
{
class CSceneSpeaker
{
private:
	CVector3f m_vPos; //喇叭位置
	std::string m_strEventName; //喇叭所播放的事件名
	float m_fMinDistance; //最小范围
	float m_fMaxDistance; //最大范围
	float m_fVolume; //音量

	std::string m_strInfo; // 标识

// 		ISound* m_pSound; //声音

public:
	bool m_bIsSceneSpeaker;

	//构造，析构
	CSceneSpeaker( );
	~CSceneSpeaker( );

	void SetPos(CVector3f& vPos);
	CVector3f* GetPos();

	void SetEventName(std::string & strEventName);
	std::string* GetEventName();

	void SetMinDistance(float fMinDistance);
	float GetMinDistance();

	void SetMaxDistance(float fMaxDistance);
	float GetMaxDistance();

	void SetVolume(float fVolume);
	float GetVolume();

	void SetInfo(std::string & strInfo);
	std::string* GetInfo();

	//void PlaySound();
	void PlaySceneSound();
	void StopSound();
};
}
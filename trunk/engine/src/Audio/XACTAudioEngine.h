#pragma once
#include "CAudioEngine.h"
#include "TSingleton.h"
#include "ThreadTypes.h"

namespace sqr
{
class CXACTAudioEngine : public CAudioEngine ,public Singleton<CXACTAudioEngine>
{
public:
	/// 取得适配器数量
	size_t GetNumAdapters() const;
	/// 返回适配器信息
	CAudioAdapterInfo GetAdapterInfo(index_t adapterIndex) const;

	/// 打开音频引擎
	bool Open();
	/// 初始化音频引擎以及注册相关事件
	bool InitializeEngine();
	void CreateAudioThread();
	/// 关闭音频引擎
	void Close();
	/// 释放globalSettingsBuffer相关值
	void ReleaseGlobalSettings();

	/// 设置全局变量
	void SetGlobalVariable(const AString& name, float value);
	/// 取得全局变量
	float GetGlobalVariable(const AString& name);
	/// 检测机子当前音效设备是否可用
	bool EstimateCurAudioDeviceIsValid(const bool lastAudioDeviceValid);

	/// 取得 XACT engine 的指针
	IXACT3Engine* GetXACTEngine() const;
	/// 等待wave bank准备完毕
	void WaitForWaveBankPrepared();

public:
	/// 构造函数
	CXACTAudioEngine();
	/// 析构函数
	virtual ~CXACTAudioEngine();

	void ReleaseEngine();
	bool RestoreXactEngine();
	void RestoreVolume();

private:
	CSoundBank* CreateSoundBank();
	CCue* CreateCue();
	CWave* CreateWave();

	bool OpenXACT();
	void CloseXACT();

	index_t GetDefaultAdapterIndex() const;

	bool LoadGlobalSettings();

private:
	/// 更新线程
	static void ThreadProc(void* pParam);
	static void WINAPI NotificationCallback(const XACT_NOTIFICATION* notification);
	static const size_t MAX_DSP_CHANNELS = 16;
	//audio device invalid的时候，特别是engine没有inialize的时候，dowork调用失败，该值为false的时候，线程不走
	static bool isNeedUpdateThread;
	static bool isCloseAudioEngine;
	static bool isDetachXactEngine;
	friend class CXACTCue;
private:
	IXACT3Engine* xactEngine;
	void* globalSettingsBuffer;
	DWORD globalSettingsBufferSize;
	X3DAUDIO_DSP_SETTINGS dspSettings;
	FLOAT32 dspMatrixCoefficients[MAX_DSP_CHANNELS];
	FLOAT32 dspDelayTimes[MAX_DSP_CHANNELS];
	X3DAUDIO_HANDLE x3dAudioHandle;

	HANDLE waveBankPreparedEvent;
	HTHREAD updateThread;

	AMap<AString, float> mapVariableVolumes;
};

//------------------------------------------------------------------------------
/// 暂时先弄个编译时的多态
inline CAudioEngine* GetAudioEngine()
{
	return CXACTAudioEngine::GetInst();
}

//------------------------------------------------------------------------------
inline IXACT3Engine* CXACTAudioEngine::GetXACTEngine() const
{
	Ast(NULL != this->xactEngine);
	return this->xactEngine;
}

}// namespace sqr


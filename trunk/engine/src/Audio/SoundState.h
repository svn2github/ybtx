#pragma once
#include "CAudioMallocObject.h"

namespace sqr
{

/// 音频状态.
struct SoundState : public CAudioMallocObject
{
	enum State
	{
		Invalid		= 0,	
		Created		= 1<<0,	///< 创建完成
		Preparing	= 1<<1,	///< 准备中
		Prepared	= 1<<2,	///< 准备完成
		Playing		= 1<<3,	///< 播放中
		Stopping	= 1<<4,	///< 停止中
		Stopped		= 1<<5,	///< 停止
		Paused		= 1<<6	///< 暂停
	};
	typedef uint	Mask;
};

}// namespace sqr
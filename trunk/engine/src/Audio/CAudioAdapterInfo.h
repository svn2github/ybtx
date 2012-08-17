#pragma once
#include "CAudioMallocObject.h"

namespace sqr
{

/// 音频适配器(声卡)信息.
class CAudioAdapterInfo : public CAudioMallocObject
{
public:
	/// 构造函数
	CAudioAdapterInfo();

	/// 适配器ID
	void SetAdpaterId(const AString& id);
	const AString& GetAdapterId() const;

	/// 名称
	void SetDisplayName(const AString& name);
	const AString& GetDisplayName() const;

	/// 是否为默认
	void SetDefaultAdapter(bool b);
	bool IsDefaultAdapter() const;

private:
	AString adapterId;		///< 声卡设备标识
	AString displayName;		///< 显示的名字, 如"Realtek HD Audio output"
	bool isDefaultAdapter;	///< 是否为默认的适配器, 多声卡时才有用
};


//------------------------------------------------------------------------------
inline CAudioAdapterInfo::CAudioAdapterInfo()
: isDefaultAdapter(false)
{
	// empty
}

//------------------------------------------------------------------------------
inline void CAudioAdapterInfo::SetAdpaterId( const AString& id )
{
	this->adapterId = id;
}

//------------------------------------------------------------------------------
inline const AString& CAudioAdapterInfo::GetAdapterId() const
{
	return this->adapterId;
}

//------------------------------------------------------------------------------
inline void CAudioAdapterInfo::SetDisplayName( const AString& name )
{
	this->displayName = name;
}

//------------------------------------------------------------------------------
inline const AString& CAudioAdapterInfo::GetDisplayName() const
{
	return this->displayName;
}

//------------------------------------------------------------------------------
inline void CAudioAdapterInfo::SetDefaultAdapter( bool b )
{
	this->isDefaultAdapter = b;
}

//------------------------------------------------------------------------------
inline bool CAudioAdapterInfo::IsDefaultAdapter() const
{
	return this->isDefaultAdapter;
}

}// namespace sqr

#pragma once
#include "CAudioMallocObject.h"

namespace sqr
{

/// cue属性.
class CCueProperty : public CAudioMallocObject
{
public:
	/// 构造函数
	CCueProperty();

	/// cue名字
	void SetCueName(const AString& cueName);
	const AString& GetCueName() const;

	/// 是否交互式的
	void SetInteractive(bool b);
	bool IsInteractive() const;

	/// 变量数目
	void SetNumVariations(size_t n);
	size_t GetNumVariations() const;

	/// 最大实例数
	void SetMaxInstances(size_t n);
	size_t GetMaxInstances() const;

private:
	AString cueName;
	bool isInteractive;
	size_t numVariations;
	size_t maxInstances;
};


//------------------------------------------------------------------------------
inline CCueProperty::CCueProperty()
: isInteractive(false)
, numVariations(0)
, maxInstances(0)
{
	// empty
}

//------------------------------------------------------------------------------
inline void CCueProperty::SetCueName( const AString& cueName )
{
	this->cueName = cueName;
}

//------------------------------------------------------------------------------
inline const AString& CCueProperty::GetCueName() const
{
	return this->cueName;
}

//------------------------------------------------------------------------------
inline void CCueProperty::SetInteractive( bool b )
{
	this->isInteractive = b;
}

//------------------------------------------------------------------------------
inline bool CCueProperty::IsInteractive() const
{
	return this->isInteractive;
}

//------------------------------------------------------------------------------
inline void CCueProperty::SetNumVariations( size_t n )
{
	this->numVariations = 0;
}

//------------------------------------------------------------------------------
inline size_t CCueProperty::GetNumVariations() const
{
	return this->numVariations;
}

//------------------------------------------------------------------------------
inline void CCueProperty::SetMaxInstances( size_t n )
{
	this->maxInstances = n;
}

//------------------------------------------------------------------------------
inline size_t CCueProperty::GetMaxInstances() const
{
	return this->maxInstances;
}

}// namespace sqr

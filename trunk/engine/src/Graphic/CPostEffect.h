#pragma once
//------------------------------------------------------------------------------
/**
	@class CPostEffect

	后处理效果.
	每个效果由多个Pass组成, 一般上个Pass的输出做为下个Pass的输入, 以此形成一个处理流水线
 */
#include "CRefObject.h"
#include "CGraphicMallocObject.h"
namespace sqr
{
SmartPointer(CPostEffectPass);
SmartPointer(CPostEffect);

class CPostEffect
	: public CRefObject
	, public CGraphicMallocObject
{
public:
	CPostEffect();
	virtual ~CPostEffect();
	/// 返回名字
	const char* GetName() const;
	/// 设置名字
	void SetName(const string& sName);
	/// 添加一个pass
	void AddPass(const CPostEffectPassPtr& pass);
	/// 返回pass数目
	size_t GetNumPasses() const;
	/// 返回指定pass
	const CPostEffectPassPtr& GetPass(index_t passIndex) const;
	/// 清空所有pass
	void Clear();
	/// 是否为空(无效状态)
	bool IsEmpty() const;
	void SetReset(bool iReset);
	bool IsReset(void);
	/// 进行处理
	void Apply();
private:
	GString m_PostName;
	bool	m_isReset;
	GVector<CPostEffectPassPtr> passes;
};

inline bool 
CPostEffect::IsReset(void)
{
	return m_isReset;
}

inline void 
CPostEffect::SetReset(bool iReset)
{
	m_isReset = iReset;
}

//------------------------------------------------------------------------------
inline const char*
CPostEffect::GetName() const
{
	return m_PostName.c_str();
}

//------------------------------------------------------------------------------
inline void
CPostEffect::SetName( const string& sName )
{
	m_PostName = sName.c_str();
}

//------------------------------------------------------------------------------
inline size_t
CPostEffect::GetNumPasses() const
{
	return this->passes.size();
}

//------------------------------------------------------------------------------
inline const CPostEffectPassPtr&
CPostEffect::GetPass( index_t passIndex ) const
{
	Ast(passIndex < this->GetNumPasses());

	return this->passes[passIndex];
}

//------------------------------------------------------------------------------
inline bool
CPostEffect::IsEmpty() const
{
	return this->passes.empty();
}

//------------------------------------------------------------------------------
inline void
CPostEffect::Clear()
{
	this->passes.clear();
}

}// namespace sqr
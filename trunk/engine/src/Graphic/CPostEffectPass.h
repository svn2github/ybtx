#pragma once
//------------------------------------------------------------------------------
/**
	@class CPostEffectPass

	后处理效果的基本组成单位.
	每个pass只有一个目标输出, 但可以有多个输入.
	如果source为空, 则取上一个pass的target
	如果target为空, 表明这是最后一个pass
 */
#include "CRefObject.h"
#include "CGraphicMallocObject.h"
#include "CShaderConstParams.h"

namespace sqr
{
SmartPointer(CRenderTexture);
SmartPointer(CPostEffectPass);
class CGpuProgram;


class CPostEffectPass
	: public CRefObject
	, public CGraphicMallocObject
{
public:
	CPostEffectPass();
	virtual ~CPostEffectPass();

	/// 设置上一个Pass
	void SetPrePass(CPostEffectPass* pass);
	/// 设置下一个Pass
	void SetNextPass(CPostEffectPass* pass);
	/// 设置目标
	void SetTarget(const CRenderTexturePtr& texture);
	/// 返回目标
	const CRenderTexturePtr& GetTarget() const;
	/// 设置输入纹理. 必须顺序添加
	void AddSource(const CRenderTexturePtr& texture);
	/// 返回输入纹理的数目
	const size_t GetNumSources() const;
	/// 返回指定输入纹理
	const CRenderTexturePtr& GetSource(index_t sourceIndex) const;
	/// 目标是否需要清空
	bool IsTargetNeedClear() const;
	/// 设置目标是否需要清空
	void SetTargetNeedClear(bool clear);
	/// 设置效果
	void SetEffect(const SHADER_HANDLE& shader);
	/// 是否最后一个Pass
	bool IsLast() const;

	/// 进行处理
	virtual void Apply();
	void UpdateViewSize();

private:
	CPostEffectPass* prePass;
	CPostEffectPass* nextPass;
	CRenderTexturePtr target;
	GVector<CRenderTexturePtr> sources;
	bool needClear;
	CGpuProgram* effect;
	SHADER_HANDLE	m_shader_handle;
	float m_invViewSize[2];
};

//------------------------------------------------------------------------------
inline void
CPostEffectPass::SetPrePass( CPostEffectPass* pass )
{
	this->prePass = pass;
}

//------------------------------------------------------------------------------
inline void
CPostEffectPass::SetNextPass( CPostEffectPass* pass )
{
	this->nextPass = pass;
}

//------------------------------------------------------------------------------
inline void
CPostEffectPass::SetTarget( const CRenderTexturePtr& texture )
{
	this->target = texture;
	this->UpdateViewSize();
}

//------------------------------------------------------------------------------
inline const CRenderTexturePtr&
CPostEffectPass::GetTarget() const
{
	return this->target;
}

//------------------------------------------------------------------------------
inline void
CPostEffectPass::AddSource( const CRenderTexturePtr& texture )
{
	this->sources.push_back(texture);
}

//------------------------------------------------------------------------------
inline const size_t
CPostEffectPass::GetNumSources() const
{
	return this->sources.size();
}

//------------------------------------------------------------------------------
inline const CRenderTexturePtr&
CPostEffectPass::GetSource( index_t sourceIndex ) const
{
	return this->sources[sourceIndex];
}

//------------------------------------------------------------------------------
inline bool
CPostEffectPass::IsTargetNeedClear() const
{
	return this->needClear;
}

//------------------------------------------------------------------------------
inline void
CPostEffectPass::SetTargetNeedClear( bool clear )
{
	this->needClear = clear;
}

//------------------------------------------------------------------------------
inline void
CPostEffectPass::SetEffect( const SHADER_HANDLE& shader )
{
	m_shader_handle = shader;
}

//------------------------------------------------------------------------------
inline bool
CPostEffectPass::IsLast() const
{
	return (NULL == this->nextPass || !this->target.isvalid());
}

}// namespace sqr

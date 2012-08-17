#pragma once

#include "CPostProcessManager.h"

namespace sqr
{

	SmartPointer(CRenderTexture);
	SmartPointer(CPostEffect);
	SmartPointer(CPostEffectPass);
	SmartPointer(CPostEffectLoader);

	class CEditPostProcessManager :public CPostProcessManager
	{
	public:
		CEditPostProcessManager(CPostProcessManager* pPostProcessMgr);
		~CEditPostProcessManager(void);

		static CEditPostProcessManager* GetInst();

		/// 初始化后处理效果
		void Setup();

		/// 释放资源
		void Discard();
		/// 返回指定效果是否启用
		bool IsEffectEnabled(const string& name);

		/// 启用/禁用指定效果
		void SetEffectEnabled(const string& name, bool enable);

		void DoPostProcess();

		CRenderTexture* GetDefaultRenderTarget();

		//新增后处理框架方法 by fyz
		bool AddEffect(CPostEffectPtr effect);
		void DelEffect(const string& name);
		bool FindEffect(const string& name);
		CPostEffectPtr GetEffect(const string& name);

		CPostProcessManager* m_pPostProcessMgr;

	};

}

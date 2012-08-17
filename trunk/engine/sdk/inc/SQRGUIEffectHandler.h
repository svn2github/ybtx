#pragma once
/*
	SQRGUIEffectHandler 
	GUI控件效果的控制器接口，控制GUI效果
*/
#include "SQRGUIEffectBase.h"
#include "SQRGuiMallocObject.h"
#include "TSqrAllocator.inl"
#include "SQRWnd.h"

namespace sqr
{
	class SQRGUIEffectHandler
		: public virtual CDynamicObject
		, public SQRGuiMallocObject
	{
	public:
		SQRGUIEffectHandler();
		~SQRGUIEffectHandler();

		bool	AddGUIEffect(SQRWnd* wnd, SQRGUIEffectBase* eff);	// 添加GUI效果
		void	DelGUIEffect(EGUIEffect eff);
		void	ExecuteEffects();									// 执行效果
		SQRGUIEffectBase*	GetEffect(EGUIEffect eff);

	private:
		void	_ProcessEffect();									// 清理失效的效果

	private:
		SQRGUIEffectBase* m_EffectUnion[eEff_Count];
	};

	inline SQRGUIEffectBase* SQRGUIEffectHandler::GetEffect(EGUIEffect eff)
	{
		if (eff >= 0 && eff < eEff_Count)
			return m_EffectUnion[eff];
		else
			return NULL;
		
	}
}
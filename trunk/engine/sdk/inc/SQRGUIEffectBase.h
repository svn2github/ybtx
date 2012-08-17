#pragma once
#include "SQRGuiMallocObject.h"
#include "GUIBase.h"
namespace sqr
{
	class SQRGUIEffectBase : public SQRGuiMallocObject
	{
	public:
		SQRGUIEffectBase() 
			: m_die(true)
			, m_del(false)
			, m_lock(false){}
		virtual ~SQRGUIEffectBase(){}

		virtual void		ExecuteEffect() = 0;
		virtual EGUIEffect	GetEffectType() = 0;
		bool				GetIsDie();
		virtual void		EndEffect()	= 0;
		bool				GetIsDel();
		void				SetIsDel(bool del);
		bool				GetbeLock();
		void				SetbeLock(bool lock);
	protected:
		bool m_die;
		bool m_del;
		bool m_lock;
	};

	inline bool SQRGUIEffectBase::GetIsDie()
	{
		return m_die;
	}

	inline bool SQRGUIEffectBase::GetIsDel()
	{
		return m_del;
	}

	inline void SQRGUIEffectBase::SetIsDel(bool del)
	{
		m_del = del;
	}

	inline bool SQRGUIEffectBase::GetbeLock()
	{
		return m_lock;
	}

	inline void SQRGUIEffectBase::SetbeLock(bool lock)
	{
		m_lock = lock;
	}

}
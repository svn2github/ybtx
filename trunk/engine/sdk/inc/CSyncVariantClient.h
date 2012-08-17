#pragma once
#include "CCypherVariant.h"
#include "CDynamicObject.h"


namespace sqr
{
	class CCoreObjectFollower;
	class CSyncVariantClientHolder;
	class CCypherVariantHolder;

	class CSyncVariantClient
		:public virtual CDynamicObject
		,protected CCypherVariant
	{
		friend class CCoreObjectFollower;
		friend class CSyncVariantClientHolder;
		friend class CCypherVariantHolder;
	public:
		template<typename ValueType>ValueType GetNumber()const
		{
			return CCypherVariant::GetNumber<ValueType>();
		}
		const char* GetString()const;

		CSyncVariantClient* GetMember(const char* szName)const;
		inline const CBaseCypherVariantCode* GetCode() const
		{
			return m_pCode;
		}
	private:
		virtual CCypherVariant* CreateVariant()const;
		virtual void RecycleSubTree();
		//void InitAllMember();

	};
}

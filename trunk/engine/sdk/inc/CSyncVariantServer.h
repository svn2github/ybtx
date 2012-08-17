#pragma once
#include "CDynamicObject.h"
#include "CCypherVariant.h"

namespace sqr
{
	class CCypherVariantHolder;
	class CSyncVariantServerHolder;

	class CSyncVariantServer
		:public virtual CDynamicObject
		,private CCypherVariant
	{
		friend class CSyncVariantServerHolder;
		friend class CCypherVariantHolder;
	public:
		using CCypherVariant::GetString;
		using CCypherVariant::GetNumber;
		using CCypherVariant::IsEmptyTable;

		void SetString(const char* szValue);

		template<typename ValueType>
		void SetNumber(ValueType Value);

		CSyncVariantServer* SetMember(const char* szName);
		CSyncVariantServer* GetMember(const char* szName)const;		
		bool DelMember(const char* szName);

		void SetEmptyTable();
		
		inline CSyncVariantServer* GetParent()const
		{
			return static_cast<CSyncVariantServer*>( CCypherVariant::GetParent() );
		}
	private:
		CSyncVariantServer(CSyncVariantServerHolder* pHolder);
		~CSyncVariantServer(void);

		inline CSyncVariantServerHolder* GetHolder()const
		{
			return m_pHolder;
		}

		virtual CCypherVariant* CreateVariant()const;
		virtual void ResetDataMember();
		virtual void RecycleSubTree();
		//void InitAllMember();

		CSyncVariantServerHolder *m_pHolder;

	};
}

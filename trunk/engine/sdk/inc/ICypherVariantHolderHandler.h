#pragma once

namespace sqr
{
	class CCypherVariant;

	class ICypherVariantHolderHandler
	{
	public:
		virtual void OnMemberAdded(CCypherVariant *pVariant)  = 0;
		virtual void OnDeleteMember(CCypherVariant *pVariant)  = 0;
		virtual void OnClearTable(CCypherVariant *pVariant)  = 0;

		virtual void OnValueChanged(CCypherVariant *pVariant)  = 0;
		virtual ~ICypherVariantHolderHandler(){}
	};
}

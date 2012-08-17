#pragma once
#include "BaseDefs.h"
#include "ICypherVariantHolderHandler.h"

NAMESPACE_BEGIN(sqr);
class CCypherVariant;

class CVariantNotifier : public ICypherVariantHolderHandler
{
public:
	virtual void OnMemberAdded(CCypherVariant *pVariant) const = 0;
	virtual void OnDeleteMember(CCypherVariant *pVariant) const = 0;
	virtual void OnValueChanged(CCypherVariant *pVariant) const = 0;

private:
	virtual void OnClearTable(CCypherVariant *pVariant) const;
};

NAMESPACE_END;
#include "stdafx.h"
#include "CSyncVariantClient.h"
#include "CCypherVariant.inl"

CCypherVariant* CSyncVariantClient::CreateVariant() const
{
	return new CSyncVariantClient;
}

void CSyncVariantClient::RecycleSubTree()
{
	IntRecycleSubTree<CSyncVariantClient>();
}

//void CSyncVariantClient::InitAllMember()
//{
//	CCypherVariant::InitAllMember(false);
//}

const char* CSyncVariantClient::GetString() const
{
	return CCypherVariant::GetString();
}

CSyncVariantClient* CSyncVariantClient::GetMember(const char* szName) const
{
	return static_cast<CSyncVariantClient*>( CCypherVariant::GetMember(szName) );
}


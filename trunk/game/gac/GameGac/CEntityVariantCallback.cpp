#include "stdafx.h"
#include "CEntityVariantCallback.h"
#include "CEntityClient.h"
#include "CSyncVariantClient.h"

int CEntityBottomSizeVariantCallback::exec(CCypherVariant *pVariant)
{
	float fBottomSize = pVariant->GetNumber<float>();
	m_pEntity->SetBottomSize(fBottomSize);
	//cout << m_pEntity->GetEntityID() << " bottomsize " << fBottomSize << endl;
	return 0;
}

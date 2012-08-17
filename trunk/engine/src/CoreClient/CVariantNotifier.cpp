#include "stdafx.h"
#include "CVariantNotifier.h"
#include "CCypherVariant.h"

namespace CVariantNotifierPrivate
{
	struct NotifyDelete
	{
		NotifyDelete( const CVariantNotifier *pNotifier ) { m_pNotifier = pNotifier; }
		void operator() ( CCypherVariant* pVariant ) const
		{
			if ( pVariant->GetType() == eVT_Table )
			{
				pVariant->ForEachMember<const NotifyDelete&>( *this );
				m_pNotifier->OnDeleteMember( pVariant );
			}
			else
			{
				m_pNotifier->OnDeleteMember( pVariant );
			}
		}
	private:
		const CVariantNotifier *m_pNotifier;
	};
};
using namespace CVariantNotifierPrivate;
void CVariantNotifier::OnClearTable(CCypherVariant *pVariant) const
{
	NotifyDelete notifyDelete( this );
	pVariant->ForEachMember<NotifyDelete&>( notifyDelete );
}
#pragma once
#include "CEntityMallocObject.h"

/*
* =====================================================================================
*        Class:  TEntityAdapterClient
*  Description:  TEntityAdapterClient
* =====================================================================================
*/
template < class Traits >
class TEntityAdapterClient 
	: public Traits::CoreObjectHandlerType, public Traits::ActorHandlerType, public Traits::FighterHandlerType, public Traits::RenderHandlerType
	, public CEntityMallocObject
{
public:

	/* ====================  LIFECYCLE     ======================================= */
	explicit TEntityAdapterClient(typename Traits::EntityType *pEntity);                             /* constructor */
	virtual ~TEntityAdapterClient();

	/* ====================  OPERATORS     ======================================= */

	/* ====================  OPERATIONS    ======================================= */
	virtual typename Traits::EntityType* GetEntity() const { return m_pEntity; }
	virtual void OnDestroy(){}

	/* ====================  INQUIRY       ======================================= */

	/* ====================  DATA MEMBERS  ======================================= */

	/* ====================  ACCESS        ======================================= */

private:
	typename Traits::EntityType* m_pEntity;

}; /* ----------  end of template class  TEntityAdapterClient  ---------- */

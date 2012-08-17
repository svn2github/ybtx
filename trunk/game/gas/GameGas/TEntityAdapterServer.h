#pragma once

#include "CEntityMallocObject.h"

/*
* =====================================================================================
*        Class:  TEntityAdapterServer
*  Description:  TEntityAdapterServer
* =====================================================================================
*/
template < class Traits >
class TEntityAdapterServer 
	: public Traits::CoreObjectHandlerType, public Traits::ActorHandlerType, public Traits::FighterHandlerType
	, public CEntityMallocObject
{
public:

	/* ====================  LIFECYCLE     ======================================= */
	explicit TEntityAdapterServer(typename Traits::EntityType *pEntity);                             /* constructor */
	virtual ~TEntityAdapterServer();

	/* ====================  OPERATORS     ======================================= */

	/* ====================  OPERATIONS    ======================================= */
	virtual typename Traits::EntityType* GetEntity() const { return m_pEntity; }

	/* ====================  INQUIRY       ======================================= */

	/* ====================  DATA MEMBERS  ======================================= */

	/* ====================  ACCESS        ======================================= */

private:
	typename Traits::EntityType* m_pEntity;

}; /* ----------  end of template class  TEntityAdapterServer  ---------- */

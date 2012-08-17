#include "stdafx.h"
#include "CSlowSigner.h"

CSlowSigner::CSlowSigner(const guid_t& guidConn)
{
	memcpy( m_guidConn , guidConn , sizeof( guid_t ) );

	m_uModSignCounter		= 0;
}

CSlowSigner::~CSlowSigner(void)
{
}

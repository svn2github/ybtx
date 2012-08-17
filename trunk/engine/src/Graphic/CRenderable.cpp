#include "stdafx.h"
#include "CRenderable.h"


CRenderable::CRenderable(ILogicHandler* pHandler)
: m_pLogicHandler(pHandler),m_isVisible(true)
{

}

CRenderable::~CRenderable()
{

}

const CAxisAlignedBox	CRenderable::GetAABB()
{
	return m_OrgAABB;
}

void CRenderable::SetVisible( bool isVisible )
{
	m_isVisible = isVisible;
}
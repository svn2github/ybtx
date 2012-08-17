#include "stdafx.h"
#include "CMirrorQuadObject.h"
#include "CCoreQuadObject.h"

CMirrorQuadObject::CMirrorQuadObject(uint32 uObjId, const CFPos &pos)
{
	m_pQuadObject = new CCoreQuadObject(uObjId, pos);
}

CMirrorQuadObject::~CMirrorQuadObject()
{

}


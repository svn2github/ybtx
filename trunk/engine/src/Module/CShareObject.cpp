#include "stdafx.h"
#include "CShareObject.h"
#include "ThreadHelper.inl"

uint32 volatile CShareObject::ms_uObjectID = 0;

CShareObject::CShareObject()
: m_uID(GetNextObjectID())
{

}

CShareObject::~CShareObject()
{

}

uint32 CShareObject::GetNextObjectID()
{
	return Atomic_FetchAndAdd((int32 volatile*)&ms_uObjectID, 1);
}

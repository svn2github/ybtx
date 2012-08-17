#include "stdafx.h"
#include "CSyncWatchHandler.h"
#include "CCoreServerObjResult.h"
#include "CCoreServerResult.inl"

void CSyncWatchHandler::OnObserveeEntered(uint32 uObjID)
{
	(new CCoreObjOnObserveeEnteredResult(uObjID, m_uID))->Add();
}

void CSyncWatchHandler::OnObserveeLeft(uint32 uObjID)
{
	(new CCoreObjOnObserveeLeftResult(uObjID, m_uID))->Add(); 
}


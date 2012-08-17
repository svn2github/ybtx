#include "stdafx.h"
#include "CLogicThreadResult.h"
#include "CCoreObjectServer.h"
#include "ExpHelper.h"
#include "CCoreObjectDictator.h"
#include "CCoreServerObjJob.h"
#include "CCoreServerJob.inl"

CAoiViewChangedResultToLogic::CAoiViewChangedResultToLogic(uint32 uMyCoreObjID, uint32 uOtCoreObjID, bool bSee, uint32 uDimension)
:CAoiRelationChangedResultToLogic(uMyCoreObjID, uOtCoreObjID, bSee, uDimension)
{
}

CAoiViewChangedResultToLogic::~CAoiViewChangedResultToLogic()
{
	CCoreObjectServer* pOtCoreObj  = CCoreObjectServer::GetObjectServer(m_uOtCoreObjID);
	Ast(pOtCoreObj);
	CCoreObjectServer* pMyCoreObj = CCoreObjectServer::GetObjectServer(m_uMyCoreObjID);
	Ast(pMyCoreObj);
	CCoreObjectDictator* pMyDicObj = pMyCoreObj->CastToObjDictator();
	Ast(pMyDicObj);
	pMyDicObj->OnViewRelationChanged(pOtCoreObj, m_bSee, m_uDimension);
}

CAoiObjHaltedResultToLogic::CAoiObjHaltedResultToLogic(uint32 uCoreObjID)
:m_uCoreObjID(uCoreObjID)
{

}

CAoiObjHaltedResultToLogic::~CAoiObjHaltedResultToLogic()
{
	(new CAsynCoreObjHaltedJob(m_uCoreObjID))->Add();
}

#include "stdafx.h"
#include "CCoreObjectStress.h"
#include "CConnStress.inl"
#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasObjectDef.h"
#include "TPackedPos.inl"

CCoreObjectStress::CCoreObjectStress()
{
	MainObject() = this;
}

CCoreObjectStress::~CCoreObjectStress()
{
	MainObject() = NULL;
}

//单件模式，创建唯一的实例
CCoreObjectStress *&CCoreObjectStress::MainObject()
{
	static CCoreObjectStress *ls_pInst = NULL;
	return ls_pInst;
}

void CCoreObjectStress::OnServerCommand(CConnStress *pConn, const sqr::CGas2GacOC_Create_Still_Object *pCmd)
{
}

void CCoreObjectStress::SetBeginPosition(CConnStress* pConn, float x, float y)
{
	CGac2GasOC_Move_Mediator_To cmd;
	cmd.fSpeed = 50;
	cmd.EndPixelPos = TPos<float>(x,y);
	cmd.fEndDist = 1000;
	cmd.usbFindPathType = 0;
	cmd.usbBarrierType = 0;

	cmd.udbPathLen = 0;
	pConn->SendStressCmd(&cmd);
}

void CCoreObjectStress::SetEndPosition(CConnStress* pConn, float x, float y)
{
	CGac2GasOC_Tell_Mediator_Reached cmd;
	cmd.uqbTimeStamp = (uint32)10;
	cmd.EndPixelPos=TPos<float>(x,y);
	
	pConn->SendStressCmd(&cmd);
}

void CCoreObjectStress::MoveTo(CConnStress* pConn, float x, float y)
{
	CGac2GasOC_Tell_Mediator_SetPosition cmd;
	cmd.PixelPos.x = x;
	cmd.PixelPos.y = y;

	pConn->SendStressCmd(&cmd);
}

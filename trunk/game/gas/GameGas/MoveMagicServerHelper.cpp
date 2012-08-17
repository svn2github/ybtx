#include "stdafx.h"
#include "CPos.h"
#include "CoreCommon.h"

void CalTriangleVertex(const CFPos& posFrom, const CFPos& posTo, CFPos& posBegin, CFPos& posLeft, CFPos& posRight)
{	
	uint32 uRadius = 5*eGridSpan;
	//m_uCount = 1;

	float fX = posTo.x - posFrom.x;
	float fY = posTo.y - posFrom.y;
	float fDistance = sqrt( fX*fX + fY*fY );
	float fSinDis = fY/fDistance;
	float fCosDis = fX/fDistance;

	float fSinAngle = float(sin(30.0*3.14159/180));
	float fCosAngle = float(cos(30.0*3.14159/180));
	float fSinLeft = fSinDis*fCosAngle + fCosDis*fSinAngle;
	float fCosLeft = fCosDis*fCosAngle - fSinDis*fSinAngle;
	float fSinRight = fSinDis*fCosAngle - fCosDis*fSinAngle;
	float fCosRight = fCosDis*fCosAngle + fSinDis*fSinAngle;

	float fLenght = uRadius * 1.732f;	// ¸ùºÅ3

	posBegin.x = posTo.x - uRadius*fCosDis;
	posBegin.y = posTo.y - uRadius*fSinDis;

	posLeft.x = posBegin.x + fLenght*fCosLeft;
	posLeft.y = posBegin.y + fLenght*fSinLeft;

	posRight.x = posBegin.x + fLenght*fCosRight;
	posRight.y = posBegin.y + fLenght*fSinRight;

}


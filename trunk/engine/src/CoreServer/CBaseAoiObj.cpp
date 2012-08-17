#include "stdafx.h"
#include "CBaseAoiObj.h"
#include "CPos.h"

void CBaseAoiObj::CalSeeRelation
(
 bool& bASeeB,			bool& bBSeeA,				/*A能否看見B，B能否看見A*/
 const CFPos& AAoiPos,	const CFPos& BAoiPos,		/*A格子位置，B格子位置*/
 float fAEyeSight,		float fBEyeSight,			/*A視距，B視距*/
 float fASize,			float fBSize,				/*A的大小，B的大小*/
 float fAKeenness,		float fBKeenness,			/*A的敏銳度，B的敏銳度*/
 float fAStealth,		float fBStealth,			/*A的隱身能力，B的隱身能力*/
 uint32 uAViewGroup,	uint32 uBViewGroup			/**/
 )
{
	float fASeeBDist = fAEyeSight + fBSize;
	float fBSeeADist = fBEyeSight + fASize;

	if( (uBViewGroup == 0) || (uAViewGroup != uBViewGroup) )
	{
		//计算隐身对发现距离的影响
		float fASeeBDistDec=fBStealth-fAKeenness;
		float fBSeeADistDec=fAStealth-fBKeenness;

		//如果敏锐大于隐身能力，则相当于完全不隐身。敏锐度再高，也不应该增加对完全不隐身对象的可视距离

		if(fASeeBDistDec<0)
			fASeeBDistDec=0;
		if(fBSeeADistDec<0)
			fBSeeADistDec=0;

		fASeeBDist-=fASeeBDistDec;
		fBSeeADist-=fBSeeADistDec;
	}

	float fOffSetX = (float)abs(AAoiPos.x - BAoiPos.x);
	float fOffSetY = (float)abs(AAoiPos.y - BAoiPos.y);

	//如果a与b的在x或y上的距离比ASeeBDist要大，a肯定看不见b
	if (fOffSetX > fASeeBDist || fOffSetY > fASeeBDist)
	{
		bASeeB = false;
		//同样，b也看不见a
		if (fOffSetX > fBSeeADist || fOffSetY > fBSeeADist)
		{
			bBSeeA = false;
			return;
		}
	}
	if (fOffSetX > fBSeeADist || fOffSetY > fBSeeADist)
	{
		bBSeeA = false;
		if (fOffSetX > fASeeBDist || fOffSetY > fASeeBDist)
		{
			bASeeB = false;
			return;
		}
	}

	float fDist=AAoiPos.Dist(BAoiPos);
	//float fDist = AAoiPos.FastDist(AAoiPos);

	bASeeB= (fBSize>0) && (fAEyeSight>0) ? fASeeBDist>fDist : false;
	bBSeeA= (fASize>0) && (fBEyeSight>0) ? fBSeeADist>fDist : false;
}



bool CBaseAoiObj::CalSeeRelation
(
 const CFPos& AAoiPos,	const CFPos& BAoiPos,
 float fAEyeSight,		float fBSize,
 float fAKeenness,		float fBStealth,
 uint32 uAViewGroup,	uint32 uBViewGroup
 )
{
	if( fBSize <= 0 || fAEyeSight <=0 )
		return false;

	float fASeeBDist = fAEyeSight + fBSize;

	if( (uBViewGroup == 0) || (uAViewGroup != uBViewGroup) )
	{
		//计算隐身对发现距离的影响
		float fASeeBDistDec=fBStealth-fAKeenness;

		//如果敏锐大于隐身能力，则相当于完全不隐身。敏锐度再高，也不应该增加对完全不隐身对象的可视距离
		if(fASeeBDistDec<0)
			fASeeBDistDec=0;

		fASeeBDist-=fASeeBDistDec;
	}

	float fOffSetX = (float)abs(AAoiPos.x - BAoiPos.x);
	float fOffSetY = (float)abs(AAoiPos.y - BAoiPos.y);

	if (fOffSetX > fASeeBDist || fOffSetY > fASeeBDist)
	{
		return false;
	}

	float fDist=AAoiPos.Dist(BAoiPos);
	//float fDist = AAoiPos.FastDist(AAoiPos);

	return fASeeBDist>fDist;
}


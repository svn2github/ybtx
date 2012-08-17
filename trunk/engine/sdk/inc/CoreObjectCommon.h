#pragma once


namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	const extern uint32	g_uClientTellStepInterval;	
	const extern uint32	g_uDefaultFollowerDelay;
	const extern uint32	g_uPlayerAoiPrecision;
	const extern uint32	g_uDirTellMedStepMaxInterval;
	const extern float	g_fDirStepMaxSentMovedDist;
	float CalMoveStep(float fSpeed,uint32 uMilliseconds);
	float CalMoveStepInMicroSeconds(float fSpeed,uint32 uMicroseconds);
	void CalcBarrierArea(vector<CPos>& vecBarrierArea, const CFPos& pixelPosCenter, uint8 uRadius, uint32 uSceneWidth, uint32 uSceneHeight);

}

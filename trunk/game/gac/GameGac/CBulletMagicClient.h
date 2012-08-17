#pragma once
#include "CMagicClient.h"
#include "CDistortedTick.h"
#include "FightDef.h"
#include "CVector3.h"
#include "CDir.h"
#include "CoreCommon.h"
#include "TCfgSharedPtr.h"
#include "CPos.h"
#include "TConfigAllocator.h"
#include "CConfigMallocObject.h"

DefineCfgSharedPtr(CBulletMagicCfgClient)

class CBulletMagicCfgClient
	: public CConfigMallocObject
{
public:
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	
	static CBulletMagicCfgClientSharedPtr&		GetByID(uint32 uID);

	uint32			GetID()	const				{return m_uID;}
	const TCHAR*	GetFX()	const				{return m_sFX.c_str();}
	float			GetSpeed() const			{return m_fSpeed;}
	float			GetPixelSpeed() const		{return float(m_fSpeed * eGridSpanForObj);}
	float			GetGravity() const			{return m_fGravity;}
	EBulletTrackType	GetTrackType() const	{return m_eBulletTrackType;}
	const TCHAR*	GetFireSkeletalName() const {return m_strFireSkeletalName.c_str();}
	const TCHAR*	GetFinalFX() const			{return m_sFinalFX.c_str();}

private:
	typedef map<uint32, CBulletMagicCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CBulletMagicCfgClientSharedPtr*> > >		MapBulletMagicCfgClientByID;
	static MapBulletMagicCfgClientByID			m_mapCfgByID;

	typedef map<string, EBulletTrackType, less<string>, TConfigAllocator<pair<string, EBulletTrackType> > >		MapBulletTrackType;
	static MapBulletTrackType					ms_mapBulletTrackType;
	static bool	InitMapBulletTrackType()	
	{	
		ms_mapBulletTrackType["平飞"]				= eBTT_Line;
		ms_mapBulletTrackType["抛物线"]				= eBTT_Parabola;
		ms_mapBulletTrackType["弹球爆魂"]			= eBTT_MarblesSoul;
		return true;
	}

	uint32				m_uID;					// 编号
	string				m_sFX;					// 特效
	float				m_fSpeed;				// 速度
	float				m_fGravity;				// 重力加速度
	EBulletTrackType	m_eBulletTrackType;     // 轨迹
	string				m_strFireSkeletalName;	// 出手骨骼
	string				m_sFinalFX;				// 最终特效
};


class CBulletMagicClient
	: public CMagicClient
	, public CDistortedTick
{
public:
	CBulletMagicClient(CCoreObjectFollower* pCoreObjFol, uint32 uSrcEntityID, uint32 uTargetEntityID, const CBulletMagicCfgClientSharedPtr& pCfg, CFPos pos, EBurstSoulType eBurstSoulType = eBST_Normal);
	~CBulletMagicClient(); 
	virtual void PlayMagicFx(const string& strMagicFx);

private:
	void OnTick();
	void Relase();
	CFPos GetDstPixelPos() const;
	float GetCurDistance() const;
	CDir GetCurDir() const;

	void UpdateTrackOfLine();
	void UpdateTrackOfParabola();
	void UpdateTrackOfMarblesSoul();
	//void OnPosChanged(){}
	void OnRefreshRenderObjPos(const CFPos& PixelPos){}
	CBulletMagicCfgClientSharedPtr m_pCfg;
	uint32 m_uTargetEntityID;
	uint32 m_uOwnerEntityID;
	float m_fHeightChangeSpeed;	//子弹高度改变速度，每次tick应改变多少高度
	
	CVector3f m_vSrcPosition;	// 子弹初始位置
	CVector3f m_vCurPosition;	// 子弹当前位置
	CVector3f m_vDstPosition;	// 子弹目的位置

	float m_fHeightSrcSpeed;	// 垂直初始速度

	CFPos m_posJumpPoint;
	float m_fJumpPointRadian;
	bool m_bJump;
	float m_fCurPlaneSpeed;
	
	uint32 m_uPassTime;
	EBurstSoulType m_eBurstSoulType;
	float m_fRealPixelSpeed;
	float m_fRealGravity;
	float m_fPlaneAcc;
};

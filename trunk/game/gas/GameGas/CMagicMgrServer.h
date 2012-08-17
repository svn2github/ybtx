#pragma once
#include "FightDef.h"
#include "CPos.h"
#include "CMagicMallocObject.h"
#include "TMagicAllocator.h"

class CMagicServer;
class CFighterDictator;
class CBulletMagicServer;
class CMoveMagicServer;
class CTransferableMagicServer;
class CShockWaveMagicServer;
class CPositionMagicServer;
class CTotemMagicServer;
class CAureMagicServer;
class CBattleArrayStationServer;
class CShockWaveMagicHandlerServer;

class CMagicMgrServer : public CMagicMallocObject
{
public:
	typedef map<EStanceType, CAureMagicServer*, less<EStanceType>, 
		TMagicAllocator<pair<const EStanceType, CAureMagicServer*> > >	MapAureMagic_t;
	typedef vector<pair<uint32,CFPos>, TMagicAllocator<pair<uint32,CFPos> > > VecBattleArray_t;
	typedef set<CAureMagicServer*, less<CAureMagicServer*>, TMagicAllocator<CAureMagicServer*> > SetAureMagic_t;

	CMagicMgrServer(CFighterDictator* pFighter);
	~CMagicMgrServer();
	uint32 AddMagic(CMagicServer* pMagic);
	void	RemoveMagic(uint32 uCheckID);
	bool	IsValid(uint32 uCheckID);
	CPositionMagicServer* GetPositionMagic()const					{return m_pPosition;}
	void SetPositionMagic(CPositionMagicServer* pPositionMagic)		{m_pPosition = pPositionMagic;}
	CMoveMagicServer* GetMoveMagic()const						{return m_pMove;}
	void SetMoveMagic(CMoveMagicServer* pMoveMagic)				{m_pMove=pMoveMagic;}
	CFighterDictator* GetFighter()								{return m_pFighter;}
	MapAureMagic_t& GetAureMagicMap()								{return m_mapAureMagic;}	
	SetAureMagic_t& GetAureMagicSet()					{return m_setAureMagic;}	
	void OnCastMagicSkill(uint32 uMagicID, EMagicGlobalType eType, bool bLoop);
	void OnCancelMagicSkill();
	void SetBattleArrayVector(VecBattleArray_t& vecBattleArray)	{m_vecBattleArray=vecBattleArray;}
	const VecBattleArray_t& GetBattleArrayVector()const			{return m_vecBattleArray;}
	void SetArrayStation(CBattleArrayStationServer* pArrayStation){m_pArrayStation=pArrayStation;}
	CBattleArrayStationServer* GetArrayStation()const			{return m_pArrayStation;}
	CShockWaveMagicHandlerServer* GetShockwaveMagic(const string& strName)const;
	void SetArrayName(const string& strArrayName);
	const string& GetArrayName()const							{return m_strArrayName;}
	CPositionMagicServer* GetPositionMagicByName(const string& strName)const;

private:
	typedef map<uint32, CMagicServer*, less<uint32>, 
		TMagicAllocator<pair<const uint32, CMagicServer*> > >	MapMagicServer_t;

	MapMagicServer_t			m_mapMagic;
	CMoveMagicServer*			m_pMove;
	CTransferableMagicServer*	m_pTransfer;
	CPositionMagicServer*		m_pPosition;
	CTotemMagicServer*			m_pTotem;
	CFighterDictator*			m_pFighter;
	MapAureMagic_t				m_mapAureMagic;
	SetAureMagic_t				m_setAureMagic;
	string						m_strArrayName;
	VecBattleArray_t			m_vecBattleArray;
	CBattleArrayStationServer*	m_pArrayStation;
	uint32						m_uCheckID;
};

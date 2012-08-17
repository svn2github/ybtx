#include "stdafx.h"
#include "CFightSkillProf.h"
#include "CMagicEffServer.h"
#include "CSkillMgrServer.h"
#include "CAppServer.h"

CFightSkillProf* CFightSkillProf::GetProf()
{
	return GetInst();
}

CFightSkillProf::CFightSkillProf()
:m_fstrmProf(NULL)
{
}

CFightSkillProf::~CFightSkillProf()
{
}

void CFightSkillProf::TurnOnFightSkillProf(uint32 uInterval)
{
	m_fstrmProf = new ofstream("FightSkillProf.log");
	CAppServer::Inst()->RegisterTick(this, uInterval);
	m_vecLastData.clear();
}

void CFightSkillProf::TurnOffFightSkillProf()
{
	m_vecLastData.clear();
	m_fstrmProf->close();
	delete m_fstrmProf;
	CAppServer::Inst()->UnRegisterTick(this);
}

void CFightSkillProf::OnTick()
{
	streambuf* oldbuf = cout.rdbuf(m_fstrmProf->rdbuf());

	if(m_vecLastData.empty())
	{
		m_vecLastData.push_back(CSkillMgrServer::GetPlayerDoSkillSuccNum());
		m_vecLastData.push_back(CSkillMgrServer::GetPlayerDoSkillFailNum());
		m_vecLastData.push_back(CSkillMgrServer::GetNpcDoSkillNum());
		m_vecLastData.push_back(CMagicEffServer::GetMagicEffExecNum());
		m_vecLastData.push_back(CMagicOpTreeServer::GetMagicOpExecNum());

		cout.rdbuf(oldbuf);
		return;
	}

	uint32 uLastSkillSuccNum = m_vecLastData[0];
	uint32 uLastSkillFailNum = m_vecLastData[1];
	uint32 uLastNpcDoSkillNum = m_vecLastData[2];
	uint32 uLastMagicEffExecNum = m_vecLastData[3];
	uint32 uLastMagicOpExecNum = m_vecLastData[4];
	m_vecLastData.clear();

	uint32 uCurSkillSuccNum = CSkillMgrServer::GetPlayerDoSkillSuccNum();
	uint32 uCurSkillFailNum = CSkillMgrServer::GetPlayerDoSkillFailNum();
	uint32 uCurNpcDoSkillNum = CSkillMgrServer::GetNpcDoSkillNum();
	uint32 uCurMagicEffExecNum = CMagicEffServer::GetMagicEffExecNum();
	uint32 uCurMagicOpExecNum = CMagicOpTreeServer::GetMagicOpExecNum();

	m_vecLastData.push_back(uCurSkillSuccNum);
	m_vecLastData.push_back(uCurSkillFailNum);
	m_vecLastData.push_back(uCurNpcDoSkillNum);
	m_vecLastData.push_back(uCurMagicEffExecNum);
	m_vecLastData.push_back(uCurMagicOpExecNum);
	

	cout << endl << "*****************************************************" << endl;
	cout << "SkillSucc: " 	<< "total " << uCurSkillSuccNum << "\t" 
							<< "per_second " << (uCurSkillSuccNum - uLastSkillSuccNum) * 1000 / GetInterval()
							<< endl;
	cout << "SkillFail: "	<< "total " << uCurSkillFailNum << "\t"
							<< "per_second " << (uCurSkillFailNum - uLastSkillFailNum) * 1000 / GetInterval()
							<< endl;
	cout << "NpcSkill: " 	<< "toatl " << uCurNpcDoSkillNum << "\t"
							<< "per_second " << (uCurNpcDoSkillNum - uLastNpcDoSkillNum) * 1000 / GetInterval()
							<< endl;
	cout << "MagicEff: "	<< "total " << uCurMagicEffExecNum << "\t"
							<< "per_second " << (uCurMagicEffExecNum - uLastMagicEffExecNum) * 1000 / GetInterval()
							<< endl;
	cout << "MagicOp: "		<< "total " << uCurMagicOpExecNum << "\t"
							<< "per_second " << (uCurMagicOpExecNum - uLastMagicOpExecNum) * 1000 / GetInterval()
							<< endl;

	cout.rdbuf(oldbuf);
}

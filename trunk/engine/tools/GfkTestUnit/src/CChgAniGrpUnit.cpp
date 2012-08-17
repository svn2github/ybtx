#include "stdafx.h"
#include "CChgAniGrpUnit.h"

void Update_WorkDir( AutoDataHelper* pUnit )
{
	((CChgAniGrpUnit*)pUnit)->UpdateWorkDir();
}

IMP_TEST_UNIT(CChgAniGrpUnit,"改变对象动作")
CChgAniGrpUnit::CChgAniGrpUnit(CEditContext* pContext)
: CBaseUnit(pContext)
{
	m_Index = 0;
	_RegCtrlData(E_STRINGBUTTON,Update_WorkDir,this,"动画路径","文件夹|*.",&m_strWorkPath);
}

CChgAniGrpUnit::~CChgAniGrpUnit()
{

}

void	CChgAniGrpUnit::_OnTick(void)
{
	if(m_pModelPool && !m_pModelPool->empty()  && m_WorkDir.GetFileNum() > 0)
	{
		if(m_Index >= m_pModelPool->size())
			m_Index = 0;

		CTestModelUnit& pModel = (*m_pModelPool)[m_Index++];
		if(!pModel.IsValid() || pModel.m_strPieceName.empty())
			return;

		if(pModel->GetSketetalFrame() == NULL)
		{
			string ani = pModel.m_strPieceName;
			ani = ani.substr(0,ani.find('.'));
			if(m_FileList.find(ani)!=m_FileList.end())
			{
				ani = m_strWorkPath + ani + ".ske";
				pModel->AddPiece(RFT_ARA,ani.c_str(),NULL,NULL);
			}
		}

		CAnimationGroup* pAniGrp = pModel->GetAnimateGroup();
		if(pModel->GetSketetalFrame() && pAniGrp)
		{
			pModel->DoAni(pAniGrp->GetAnimationName(0),true);
		}
	}
}

void sqr_tools::CChgAniGrpUnit::UpdateWorkDir( void )
{
	if(m_strWorkPath.empty())
		return;

	string tmp = m_strWorkPath;
	replace(tmp.begin(),tmp.end(), '/','\\');
	m_strWorkPath  = tmp.substr( 0,tmp.find_last_of('\\') + 1);
	m_WorkDir.Open(PATH_ALIAS_RES.c_str(), m_strWorkPath.c_str(), "agp");
	m_FileList.clear();
	for(uint32 i = 0; i< m_WorkDir.GetFileNum(); ++i)
	{
		string tmp;
		m_WorkDir.GetFileName(i,tmp);
		tmp = tmp.substr(0,tmp.find('.'));
		m_FileList.insert(tmp);
	}
}
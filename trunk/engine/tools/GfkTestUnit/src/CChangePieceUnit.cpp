#include "stdafx.h"
#include "CChangePieceUnit.h"
#include "CEditModel.h"

void Update_Work_Dir(AutoDataHelper* pUnit)
{
	((CChangePieceUnit*)pUnit)->UpdateWorkDir();
}

IMP_TEST_UNIT(CChangePieceUnit,"改变对象模型")
CChangePieceUnit::CChangePieceUnit(CEditContext* pContext)
: CBaseUnit(pContext)
{
	m_strWorkPath = "";
	m_bIsRandom = true;
	m_bIgnoreHasPiece = true;
	_RegCtrlData(E_STRINGBUTTON,Update_Work_Dir,this,"模型路径","文件夹|*.",&m_strWorkPath );
	_RegCtrlData(E_BOOLValue,NULL,NULL,"随机创建",NULL,&m_bIsRandom);
	_RegCtrlData(E_BOOLValue,NULL,NULL,"只填装一次",NULL,&m_bIgnoreHasPiece);
	m_uLastIndex = 0;
}

CChangePieceUnit::~CChangePieceUnit()
{
}

void CChangePieceUnit::UpdateWorkDir(void)
{
	if(m_strWorkPath.empty())
		return;

	string tmp = m_strWorkPath;
	replace(tmp.begin(),tmp.end(), '/','\\');
	m_strWorkPath  = tmp.substr( 0,tmp.find_last_of('\\') + 1);

	m_WorkDir.Open(PATH_ALIAS_RES.c_str(), m_strWorkPath.c_str(), "mod");
}

void CChangePieceUnit::_OnTick(void)
{
	if(m_pModelPool && !m_pModelPool->empty() && m_WorkDir.GetFileNum() > 0 )
	{
		if(m_uModelIndex >= m_pModelPool->size())
			m_uModelIndex = 0;
		CTestModelUnit& pModel = (*m_pModelPool)[m_uModelIndex];

		if(!pModel.IsValid())
			return; 

		m_uModelIndex++;

		if( pModel.m_strPieceName.empty() || !m_bIgnoreHasPiece)
		{
			pModel->ClearMeshPiece();
			pModel->DelFramework();
			if(m_bIsRandom)
			{
				m_uLastIndex = (uint32)((float(rand()) / 32767.0f) *m_WorkDir.GetFileNum());
			}
			else
			{
				m_uLastIndex = (++m_uLastIndex)%m_WorkDir.GetFileNum();
			}

			string PieceGroupName;
			m_WorkDir.GetFullFileName(m_uLastIndex,PieceGroupName);
			m_WorkDir.GetFileName(m_uLastIndex,pModel.m_strPieceName);
			pModel->AddPiece(RFT_ARP,PieceGroupName.c_str(),NULL,NULL);
		}
	}
}
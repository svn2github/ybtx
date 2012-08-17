#include "stdafx.h"
#include "CEfxTestUnit.h"
#include "CEditModel.h"

void Update_Efx(AutoDataHelper* pUnit)
{
	((CEfxTestUnit*)pUnit)->UpdateWorkDir();
}


IMP_TEST_UNIT(CEfxTestUnit,"特效测试用例")
CEfxTestUnit::CEfxTestUnit(CEditContext* pContext)
: CBaseUnit(pContext)
, m_isErgDir(false)
, m_uCount(0)
, m_ModelIdx(0)
, m_EfxIdx(0)
{
	_RegCtrlData(E_STRINGBUTTON,Update_Efx,this,"选择特效","选择特效|*.efx",&m_EfxFile );
	_RegCtrlData(E_BOOLValue,NULL,NULL,"遍历子目录",NULL,&m_isErgDir);
	_RegCtrlData(E_INTEGER,NULL,NULL,"增加数量",NULL,&m_uCount);
}

CEfxTestUnit::~CEfxTestUnit()
{

}

void	CEfxTestUnit::_OnTick(void)
{
	if(m_EfxFile.empty())
		return;

	string efxName = m_EfxFile;

	for(uint32 i = 0; i < m_uCount; ++i)
	{
		if(m_EfxIdx == 0)
		{
			if(!m_ErgDirPool.empty())
			{
				while (1)
				{
					CDirNode& node = m_ErgDirPool.top();

					if(node.m_uIndex>=node.m_pDir.GetFileNum())
					{
						if(m_ErgDirPool.size() == 1)
						{
							node.m_uIndex = 0;
						}
						else
						{
							m_ErgDirPool.pop();
							continue;
						}
					}
					node.m_pDir.GetFullFileName(node.m_uIndex++,efxName);

					if(efxName.find(".dir")!=string::npos)
					{
						if(m_isErgDir)
							m_ErgDirPool.push(CDirNode(efxName));
					}
					else if( efxName.find(".efx")!=string::npos )
						break;
				}
			}
		
			m_EfxInfo.Load(efxName);
			if(m_EfxInfo.GetCount() == 0)
				return;
		}

		if(m_ModelIdx>=m_pModelPool->size())
			m_ModelIdx = 0;

		while( !(*m_pModelPool)[m_ModelIdx].IsValid() )
		{
			++m_ModelIdx;
			if(m_ModelIdx>=m_pModelPool->size())
			{
				m_ModelIdx = 0;
				break;
			}
		}

		if( 0 != (*m_pModelPool)[m_ModelIdx++]->AddEffect(efxName.c_str(),m_EfxInfo.GetEfxName(m_EfxIdx).c_str(),0,NULL) )
			if(++m_EfxIdx>=m_EfxInfo.GetCount()) m_EfxIdx = 0;
	}
}

void	CEfxTestUnit::UpdateWorkDir(void)
{
	while(!m_ErgDirPool.empty())
		m_ErgDirPool.pop();

	if(m_EfxFile.empty())
		return;

	if(m_EfxFile.find("选择打开文件") != string::npos)
	{
		string tmp = m_EfxFile;
		replace(tmp.begin(),tmp.end(), '/','\\');
		m_EfxFile  = tmp.substr( 0,tmp.find_last_of('\\') + 1);
		m_ErgDirPool.push(CDirNode(m_EfxFile));
	}
}


sqr_tools::CEfxTestUnit::CDirNode::CDirNode( const string& dir )
{
	m_uIndex = 0;
	m_pDir.Open(PATH_ALIAS_RES.c_str(), dir.c_str());
}
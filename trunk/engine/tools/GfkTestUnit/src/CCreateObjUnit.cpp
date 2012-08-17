#include "stdafx.h"
#include "CCreateObjUnit.h"
#include "CEditContext.h"

void Resize_Vector( AutoDataHelper* pUnit )
{
	((CCreateObjUnit*)pUnit)->ReSizeVector();
}

IMP_TEST_UNIT(CCreateObjUnit,"创建模型对象")
CCreateObjUnit::CCreateObjUnit(CEditContext* pContext)
: CBaseUnit(pContext)
, m_uCount(1)
, m_uNumSp(1)
, m_uTime(0)
, m_bCamera(true)
{
	_InitUnit();

	CVector3f CamPos;
	pContext->GetCameraDest(CamPos);
	m_fCenterX	= CamPos.x;
	m_fCenterY	= CamPos.z;
	m_fRange	= 0.0f;

	_RegCtrlData(E_INTEGER,Resize_Vector,this,"创建总数",NULL,&m_uCount);
	_RegCtrlData(E_INTEGER,NULL,NULL,"每分钟创建个数",NULL,&m_uNumSp);

	_RegCtrlData(E_BOOLValue,NULL,NULL,"中心点跟随摄像机",NULL,&m_bCamera);

	_RegCtrlData(E_FLOAT,NULL,NULL,"固定中心点X",NULL,&m_fCenterX);
	_RegCtrlData(E_FLOAT,NULL,NULL,"固定中心点Y",NULL,&m_fCenterY);
	_RegCtrlData(E_FLOAT,NULL,NULL,"创建半径",NULL,&m_fRange);
	m_BackIdx = 0;
	ReSizeVector();
}

CCreateObjUnit::~CCreateObjUnit()
{
	_UnInitUnit();
}

void	CCreateObjUnit::_InitUnit()
{
	if(m_pModelPool==NULL)
		return;

	for(uint32 i = 0; i < m_uCount; ++i)
	{
		m_pModelPool->push_back(_CreateNewModel());
	}
}

void	CCreateObjUnit::_UnInitUnit()
{
	if(m_pModelPool==NULL)
		return;

	ModelPool::iterator it,eit = m_pModelPool->end();
	for( it = m_pModelPool->begin(); it!=eit; ++it)
	{
		if((*it).IsValid())
			m_pContext->DestroyRenderObject(*it);
	}
	m_pModelPool->clear();
	m_pContext = NULL;
}

void	CCreateObjUnit::_OnTick(void)
{
	if(m_pContext==NULL || m_pModelPool==NULL || m_uCount == 0)
		return;

	if(m_pModelPool->size() != m_uCount)
		ReSizeVector();

	m_uTime += GetInterval();
	uint32 ChangeCount = uint32(m_uNumSp * (float(m_uTime)/( 1000.0f * 60.0f )));
	if(ChangeCount == 0)
		return;

	if(m_bCamera)
	{
		CVector3f CamPos;
		m_pContext->GetCameraDest(CamPos);
		if( m_fCenterX!=CamPos.x || m_fCenterY!=CamPos.z )
		{
			m_fCenterX	= CamPos.x;
			m_fCenterY	= CamPos.z;
			UIShowUpdate();
		}
	}

	m_uTime = 0;

	for( uint32 i = 0; i<ChangeCount; ++i )
	{
		if((*m_pModelPool)[m_BackIdx] != NULL)
		{
			m_pContext->DestroyRenderObject((*m_pModelPool)[m_BackIdx]);
			(*m_pModelPool)[m_BackIdx] = NULL;
		}

		(*m_pModelPool)[m_BackIdx++] = _CreateNewModel();
		if( m_uCount <= m_BackIdx )
		{
			m_BackIdx = 0;
		}
	}
}

CEditModel* CCreateObjUnit::_CreateNewModel( void )
{
	float	invMaxRand = 1.0f / 32768.0f;
	CEditModel* pModel = m_pContext->CreateEditModel(NULL);
	float rrx = m_fRange * ( rand() * invMaxRand - 0.5f );
	float rry = m_fRange * ( rand() * invMaxRand - 0.5f );
	m_pContext->SetRenderObjectPosition( *pModel, CVector3f(m_fCenterX + rrx, 0.0f, m_fCenterY + rry ), true);
	return pModel;
}

void sqr_tools::CCreateObjUnit::ReSizeVector( void )
{
	if(m_pModelPool == NULL)
		return;

	for(size_t i = m_uCount; i<m_pModelPool->size(); ++i)
	{
		if((*m_pModelPool)[i].IsValid())
			m_pContext->DestroyRenderObject((*m_pModelPool)[i]);
	}

	m_pModelPool->resize(m_uCount,NULL);
	m_BackIdx = limit2(m_BackIdx,uint32(0),m_uCount - 1);
}
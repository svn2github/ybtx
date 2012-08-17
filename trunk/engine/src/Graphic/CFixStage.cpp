#include "stdafx.h"
#include "CFixStage.h"

CFixStage::CFixStage(StageFlag sf)
: CStageNode(sf)
, m_eStageEx(SFE_SHADER)
{

}

void	CFixStage::SetStageEx(StageFlagEx sfe)
{
	m_eStageEx = sfe;
}
#include "stdafx.h"
#include "CGpuProgram.h"

bool CGpuProgram::isNeedUpdateParameters()
{
	return m_isNeedUpdateParameters;
}

void CGpuProgram::Update()
{
	m_isNeedUpdateParameters=false;
} 
#include "stdafx.h"
#include "CModifyBatch.h"
#include "CVertexDeclaration.h"
#include "CRenderInstBatch.h"
#include "CGpuProgramMgr.h"
#include "TSqrAllocator.inl"

CGeometryBatch* CreateHWRenderBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
{
	return new CHWRenderInstBatch(fFlag,fHandle,pParams,pParent);
}

CModifyBatch::ModifyRenderState::ModifyRenderState(STATIC_RS* RS,DWORD* KEY, DWORD VAL)
{
	Key = (size_t)((char*)KEY - (char*)RS);
	Val	= VAL;
}

void CModifyBatch::ModifyRenderState::ModifyRS(STATIC_RS&	RS)
{
	*(DWORD*)(((char*)&RS) + Key) = Val;
}

CModifyBatch::CModifyBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: CGeometryBatch(fFlag,fHandle,pParams,pParent)
, m_AddVS(0)
, m_AddPS(0)
,m_ShaderId(0)
{
	if(m_CurHandle != 0)
	{
		STATIC_RS RS;
		if((m_CurHandle & CVertexElement::GetElementSemanticID(VES_DIFFUSE,0)) != 0)
		{
			m_AddVS = MakeSysVS_ID(SSI_PRE_RENDER_LIGHT);
			m_VecModify.push_back( ModifyRenderState(&RS,&(RS.m_LightEnable),FALSE) );
		}
	}
	else
	{
		if( CGpuProgramMgr::GetInst()->GetShaderVer() >= Shader_V_3 )
		{
			m_ShaderId = MakeSysVS_ID(SSI_INSTANCE);
			m_CurCreateFlag.CreateRenderBatch = CreateHWRenderBatch;
		}
	}
}

DWORD CModifyBatch::GetAddDecl(void)
{
	return (DWORD)m_CurHandle;
}

void CModifyBatch::ModifyRS(STATIC_RS&	RS)
{
	for(size_t i = 0; i<m_VecModify.size(); ++i)
		m_VecModify[i].ModifyRS(RS);

	RS.m_ShaderType |= m_ShaderId;

	if(RS.m_ShaderType.GetVSShaderID() != 0)
		RS.m_ShaderType |= m_AddVS;

	if(RS.m_ShaderType.GetPSShaderID() != 0)
		RS.m_ShaderType |= m_AddPS;
}

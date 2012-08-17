#include "stdafx.h"
#include "CDX9ShaderMacro.h"
#include "TSqrAllocator.inl"

namespace sqr
{
const GString DX9_ShaderMacroMgr::NULLDefinition = "";
const DWORD		DX9_ShaderMacroMgr::TEST_ID[] =
{
	0x40002000,
	0x40001000,
	0x40000300,
	0x40000C00,
	0x00000C00,
	0x80000300,
};
const uint		DX9_ShaderMacroMgr::Num_TEST = sizeof(DX9_ShaderMacroMgr::TEST_ID)/sizeof(DWORD);
ShaderMacroMgr::MacroMap::iterator	DX9_ShaderMacroMgr::DefaultPreLight;
ShaderMacroMgr::MacroMap::iterator	DX9_ShaderMacroMgr::DefaultTrans;

DX9_ShaderMacroMgr::DX9_ShaderMacroMgr()
{
	BeginInit();
	InitDx9Macro(SCT_VERTEX_TRAN);
	InitDx9Macro(SCT_LIGHT_TRAN);
	InitDx9Macro(SCT_TRANSFORM_TRAN);
	InitDx9Macro(SCT_DIFFUSE_TRAN);
	InitDx9Macro(SCT_TEXTURE_TRAN);
	InitDx9Macro(SCT_INPUT_VAR);
	InitDx9Macro(SCT_PS_SHADOW);
	EndInit();
	DefaultPreLight = m_mapMacro.find(SSI_PRE_RENDER_LIGHT);
	DefaultTrans	= m_mapMacro.find(SSI_INSTANCE);
}

DX9_ShaderMacroMgr::~DX9_ShaderMacroMgr()
{}

D3DXMACRO*	DX9_ShaderMacroMgr::GET_MACRO(DWORD ID)
{
	ClearMacro(SCT_VERTEX_TRAN);
	ClearMacro(SCT_DIFFUSE_TRAN);
	ClearMacro(SCT_TEXTURE_TRAN);
	ClearMacro(SCT_INPUT_VAR);
	ClearMacro(SCT_PS_SHADOW);
	m_dxMacro[SCT_LIGHT_TRAN].Definition = DefaultPreLight->second.Definition.c_str();
	m_dxMacro[SCT_TRANSFORM_TRAN].Definition = DefaultTrans->second.Definition.c_str();

	if (ID_Test(ID,SSI_BASE_VS_SYSTEM_ID)||ID_Test(ID,SSI_BASE_PS_SYSTEM_ID))
	{
		MacroMap::iterator it;
		DWORD tempID;
		for (uint i= 0;i<Num_TEST;++i)
		{
			tempID = ID&TEST_ID[i];
			if ( (tempID&SSI_BASE_FX_SYSTEM_ID) == 0 && (TEST_ID[i]&SSI_BASE_FX_SYSTEM_ID) !=0 )
				continue;
			it = m_mapMacro.find(tempID);
			if (it!=m_mapMacro.end())
				m_dxMacro[it->second.MacroFlag].Definition = it->second.Definition.c_str();
		}
	}

	m_dxMacroOut.assign(m_dxMacro, m_dxMacro+SCT_COUNT);

	if ( !m_macDefMap.empty() )
	{
		MacroDefineMap::iterator it  = m_macDefMap.begin();
		MacroDefineMap::iterator eit = m_macDefMap.end();
		for (  ; it != eit; ++it )
		{
			D3DXMACRO mac;
			mac.Name		= it->first.c_str();
			mac.Definition	= it->second.c_str();
			m_dxMacroOut.push_back(mac);
		}
	}

	D3DXMACRO mac_end;
	mac_end.Name		= 0;
	mac_end.Definition	= 0;
	m_dxMacroOut.push_back(mac_end);

	return (D3DXMACRO*)&m_dxMacroOut[0];
}


HRESULT DX9_ShaderInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, uint *pBytes)
{
	IncludeMap::iterator it = m_incFile.find(GString(pFileName));
	if (it!=m_incFile.end())
	{
		*ppData = it->second.c_str();
		*pBytes = it->second.size();
		return S_OK;
	}
	*ppData = NULL;
	*pBytes = 0;
	return S_FALSE;
}

HRESULT DX9_ShaderInclude::Close(LPCVOID pData)
{
	return S_OK;
}
}
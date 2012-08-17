#ifndef _DX9_Shader_Macro_H_
#define _DX9_Shader_Macro_H_
#include "CShaderConstParams.h"
#include "Dx9Base.h"

namespace sqr
{
class DX9_ShaderMacroMgr : public ShaderMacroMgr
{
#define BeginInit()
#define InitDx9Macro(Type) m_dxMacro[Type].Name = SYS_MACRO_MAP[Type]->c_str();
#define EndInit() m_dxMacro[SCT_COUNT].Name = NULL; m_dxMacro[SCT_COUNT].Definition = NULL;
#define ClearMacro(Type) m_dxMacro[Type].Definition = NULLDefinition.c_str()
public:
	static const DWORD		TEST_ID[];
	static const uint		Num_TEST;
public:
	DX9_ShaderMacroMgr();
	~DX9_ShaderMacroMgr();
public:
	D3DXMACRO*	GET_MACRO(DWORD ID);
protected:
	D3DXMACRO	m_dxMacro[SCT_COUNT+1];
	GVector<D3DXMACRO> m_dxMacroOut;
	static	const GString NULLDefinition;
	static	MacroMap::iterator DefaultPreLight;
	static	MacroMap::iterator DefaultTrans;
};


class DX9_ShaderInclude
			:	public ID3DXInclude
			,	public ShaderInclude
{
public:
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, uint *pBytes);
	STDMETHOD(Close)(THIS_ LPCVOID pData);
	static DX9_ShaderInclude* GetInst() { return (DX9_ShaderInclude*)ShaderInclude::GetInst(); }
};
}

#endif
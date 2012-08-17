#pragma once
#include "Dx9Base.h"
#include "CVertexDeclaration.h"
#include "BaseHelper.h"
namespace sqr
{
class DX9VertexDeclaration : public CVertexDeclaration
{
public:
	DX9VertexDeclaration(DWORD FVF)
			: CVertexDeclaration(FVF)
			, m_pD3DDecl(NULL)
	{};

	~DX9VertexDeclaration()
	{
		SafeRelease( m_pD3DDecl );
	}

	LPDIRECT3DVERTEXDECLARATION9 getD3DVertexDecl()
	{
		return m_pD3DDecl;
	};
	void update();
protected:
	LPDIRECT3DVERTEXDECLARATION9	m_pD3DDecl;
};
}
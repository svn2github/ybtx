#include "stdafx.h"
#include "CDX9VertexDeclaration.h"
#include "CDX9RenderPipeline.h"
#include "CDX9MainWindow.h"

void DX9VertexDeclaration::update()
{
	if ( m_bIsChanged )
	{
		SafeRelease(m_pD3DDecl);
		//´´½¨D3DÔªËØ
		D3DVERTEXELEMENT9* d3delems = new D3DVERTEXELEMENT9[m_ElementList.size() + 1];

		VertexElementList::const_iterator i, iend;
		uint idx;
		iend = m_ElementList.end();
		for (idx = 0, i = m_ElementList.begin(); i != iend; ++i, ++idx)
		{
			d3delems[idx].Method = D3DDECLMETHOD_DEFAULT;
			d3delems[idx].Offset = static_cast<WORD>(i->GetOffset());
			d3delems[idx].Stream = i->GetSource();
			d3delems[idx].Type = static_cast<BYTE>(DX9RenderPipeline::get(i->GetType()));
			d3delems[idx].Usage = static_cast<BYTE>(DX9RenderPipeline::get(i->GetSemantic()));

			if (i->GetSemantic() == VES_SPECULAR)
			{
				d3delems[idx].UsageIndex = 1;
			}
			else if (i->GetSemantic() == VES_DIFFUSE)
			{
				d3delems[idx].UsageIndex = 0;
			}
			else
			{
				d3delems[idx].UsageIndex = static_cast<BYTE>(i->GetIndex());
			}
		}
		d3delems[idx].Stream = 0xff;
		d3delems[idx].Offset = 0;
		d3delems[idx].Type = D3DDECLTYPE_UNUSED;
		d3delems[idx].Method = 0;
		d3delems[idx].Usage = 0;
		d3delems[idx].UsageIndex = 0;


		if (FAILED( GetD3DDevice()->CreateVertexDeclaration(d3delems, &m_pD3DDecl) ))
		{
			Ast( "void DX9SVertexDeclaration::update()" );
		}

		delete [] d3delems;

		m_bIsChanged  = false;

	}
	return;
}

